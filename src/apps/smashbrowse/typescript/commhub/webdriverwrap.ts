import webdriver = require('selenium-webdriver')
import chrome = require('selenium-webdriver/chrome');
import Path = require('path')

import {FSWrap} from './fswrap'
import {send_msg_to_app, send_msg_to_ext, get_app_server_port, get_ext_server_port} from './commhub'
import {DebugUtils} from './debugutils'



export let Key = webdriver.Key
let By = webdriver.By
let Until = webdriver.until


export class WebDriverWrap {

    driver: webdriver.WebDriver
    flow: webdriver.promise.ControlFlow 
    fswrap: FSWrap
    iframe: string // the current iframe path as a string

    constructor(fswrap: FSWrap) {
        this.driver = null
        this.flow = null
        this.fswrap = fswrap
        this.iframe = "" // The top level window/frame as an empty iframe path.
    }

    browser_is_open(callback: (result: boolean) => void) {
        if (this.driver) {
            this.driver.getTitle().then(
                function() { callback(true) },
                function() { callback(false) });
        } else {
            callback(false)
        }
    }

    open_browser(): boolean {
        try {
            let chromeOptions = new chrome.Options()
            //Win_x64-389148-chrome-win32
            //Win-338428-chrome-win32
            //chromeOptions.setChromeBinaryPath('/downloaded_software/chromium/Win_x64-389148-chrome-win32/chrome-win32/chrome.exe')

            //let url = "file://"+FSWrap.get_bin_dir() + '/../html/smashbrowse.html?' + get_app_server_port()
            //url = url.replace(/\\/g,"/")

            let url = "https://www.google.com/?" + get_ext_server_port()
            chromeOptions.addArguments(url)
            chromeOptions.addArguments("--load-extension=" + FSWrap.get_chrome_ext_dir())
            chromeOptions.addArguments("--ignore-certificate-errors")
            chromeOptions.addArguments("--disable-web-security")
            chromeOptions.addArguments("--user-data-dir=" + this.fswrap.get_chrome_user_data_dir())
            chromeOptions.addArguments("--first-run")
            //chromeOptions.addArguments("--app=file:///"+url)

            // "C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" --disable-web-security --user-data-dir --app=https://www.google.com

            this.driver = new webdriver.Builder()
                .forBrowser('chrome')//.forBrowser('firefox')
                .setChromeOptions(chromeOptions)
                .build();

            this.flow = webdriver.promise.controlFlow()

            // Set default settings.
            this.driver.manage().timeouts().pageLoadTimeout(60000);

            webdriver.promise.controlFlow().on('uncaughtException', function(e: Error) {
                console.error('Unhandled error: ' + e);
            });

            return true
        } catch (e) {
            DebugUtils.log_exception(e)
            return false
        }
    }

    close_browser(): webdriver.promise.Promise<void> {
        return this.driver.quit()
    }

    //Returns a promise which navigates the browser to another url.
    navigate_to(url: string): webdriver.promise.Promise<void> {
        return this.driver.navigate().to(url)
    }

    //Returns a promise which navigates the browser forward in the history.
    navigate_forward(): webdriver.promise.Promise<void> {
        return this.driver.navigate().forward();
    }

    //Returns a promise which navigates the browser backwards in the history.
    navigate_back(): webdriver.promise.Promise<void> {
        return this.driver.navigate().back();
    }

    //Returns a promise which refreshes the browser.
    navigate_refresh(): webdriver.promise.Promise<void> {
        return this.driver.navigate().refresh();
    }

    switch_to_iframe(iframe: string): webdriver.promise.Promise<void> {
        this.driver.switchTo().defaultContent()
        let splits = this.iframe.split('/')
        let last_promise: webdriver.promise.Promise<void> = null
        for (let i=0; i<splits.length; i++) {
            last_promise = this.driver.switchTo().frame(splits[i])
        }
        return last_promise
    }

    //Returns a promise which resizes the browser.
    resize_browser(width: number, height: number): webdriver.promise.Promise<void> {
        return this.driver.manage().window().setSize(width, height);
    }

    //Returns a promise which jitters the browser size.
    jitter_browser_size(): webdriver.promise.Promise<{}> {
        let our_driver = <webdriver.WebDriver>this.driver
        return our_driver.manage().window().getSize().then(
            function (s: webdriver.ISize) {
                let our_driver = <webdriver.WebDriver>this.driver
                our_driver.manage().window().setSize(s.width + 1, s.height + 1).then(
                    function () {
                        let our_driver = <webdriver.WebDriver>this.driver
                        our_driver.manage().window().setSize(s.width, s.height);
                    }.bind(this)
                )
            }.bind(this)
        )
    }

    //------------------------------------------------------------------------------------------------
    // Dom Element Actions.
    //------------------------------------------------------------------------------------------------

    // Note the returned promise nevers errors.
    // It will try to wait for the xpath
    // element as long as it can.

    static trivial_wait_time = 1000 //1000
    static critical_wait_time = 1000 //30000


    //Returns a promise which evaulates to an existing element or is rejected.
    get_element(xpath: string, wait_milli: number): webdriver.promise.Promise<webdriver.WebElement> {
        return this.driver.findElement(By.xpath(xpath))
    }
        
    //Returns a promise which evaulates to a visible element.
    get_visible_element(xpath: string, wait_milli: number): webdriver.promise.Promise<{}> {
        return this.get_element(xpath, wait_milli).then(
            function (element: webdriver.WebElement) {
                let our_driver = <webdriver.WebDriver>this.driver
                return our_driver.wait(Until.elementIsVisible(element), wait_milli).then(
                    function (element) { return element },
                    function (error) { console.info('Warning: element was not visible: ' + xpath); throw error })
            }.bind(this)
        )
    }

    // Creates promise chain which will type one key into an element.
    send_key(xpath: string, key: string): void {
        let p = this.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(
            function (element: webdriver.WebElement) {
                return element.sendKeys(key)
            }.bind(this)
        )
        this.terminate_chain(p)
    }

    get_text(xpath: string) {
        let p = this.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(
            function (element: webdriver.WebElement) {
                return element.getText()
            }.bind(this)
        )
        this.terminate_chain(p)
    }

    // Creates promise chain which will set text on an element.
    send_text(xpath: string, text: string) {
        let p = this.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(
            function (element: webdriver.WebElement) {
                return element.sendKeys(Key.HOME, Key.chord(Key.SHIFT, Key.END), text)
            }.bind(this)
        )
        this.terminate_chain(p)
    }

    // Creates a promise chain which will click on an element.
    click_on_element(xpath: string) {
        console.log("clicking on element: " + xpath)
        let p = this.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(
            function (element: webdriver.WebElement) {
                let our_driver = <webdriver.WebDriver>this.driver
                return our_driver.actions().click(element).perform()
            }.bind(this)
        )
        this.terminate_chain(p)
    }

    // Creates a promise chain which will mouseover an element.
    mouse_over_element(xpath: string, relative_x: number, relative_y: number) {
        let p = this.get_visible_element(xpath, WebDriverWrap.trivial_wait_time).then(
            function (element: webdriver.WebElement) {
                let our_driver = <webdriver.WebDriver>this.driver
                return our_driver.actions().mouseMove(element, { x: relative_x, y: relative_y }).perform().then(
                    function () { },
                    function (error) { console.info('Warning: could not move_over_element (computedstyle): ' + xpath); throw (error) }
                )
            }.bind(this)
        )
        this.terminate_chain(p)
    }

    // Creates a promise which will select an option in a select dropdown.
    select_option(xpath: string, option_text: string) {
        let p = this.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(
            function (element: webdriver.WebElement) {
                return element.findElement(By.xpath('option[normalize-space(text())="' + option_text + '"]')).click()
            }.bind(this)
        )
        this.terminate_chain(p)
    }

    // Helper to terminate promise chains.
    terminate_chain<T>(p: webdriver.promise.Promise<T>) {
        p.then(
            function() {
                // Make sure the events are blocked. They may be unblocked to allow webdriver actions to take effect.
                let req = new RequestMessage(this.iframe, RequestType.kBlockEvents)
                send_msg_to_ext(req)
                // Send success response to the app.
                console.log('terminating chain success with numargs: ' + arguments.length)
                if (arguments.length == 0) {
                    send_msg_to_app(new ResponseMessage(this.iframe, true))
                } else {
                    // Send the first argument in the response.
                    send_msg_to_app(new ResponseMessage(this.iframe, true, arguments[0]))
                }
            }.bind(this),
            function(error: any) {
                // Make sure the events are blocked. They may be unblocked to allow webdriver actions to take effect.
                let req = new RequestMessage(this.iframe, RequestType.kBlockEvents)
                send_msg_to_ext(req)
                // Output error details.
                console.error("Error in chain!")
                if (error.stack.indexOf('mouse_over_element') >= 0) {
                    console.error('error from mouse_over_element')
                } else if (error.stack.indexOf('click_on_element') >= 0) {
                    console.error('error from click_on_element')
                } else if (error.stack.indexOf('send_key') >= 0) {
                    console.error('error from send_key')
                } else if (error.stack.indexOf('send_text') >= 0) {
                    console.error('error from send_text')
                } else {
                    DebugUtils.log_exception(error)
                }

                console.error('exception: ' + error.message + ' stack: ' + error.stack)

                // Send failure reponse to the app.
                send_msg_to_app(new ResponseMessage(this.iframe, false))
            }.bind(this))
    }

}