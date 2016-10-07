import webdriver = require('selenium-webdriver')
import chrome = require('selenium-webdriver/chrome')
import Path = require('path')

import {FSWrap} from './fswrap'
import {send_msg_to_app, send_msg_to_ext, get_app_server_port, get_ext_server_port} from './commhub'
import {DebugUtils} from './debugutils'

export let Key = webdriver.Key
let By = webdriver.By
let Until = webdriver.until

// This is an example of how to get a list of process ids on windows.
// Note tasklist is specific to the windows platform.
// import child_process =  require('child_process')
// let before: string
// child_process.exec('tasklist', function (err: Error, stdout: string, stderr: string) {
//     before = stdout
//     console.log('processes: ' + stdout)
// });

export class WebDriverWrap {

    driver: webdriver.WebDriver
    flow: webdriver.promise.ControlFlow 
    fswrap: FSWrap

    constructor(fswrap: FSWrap) {
        this.driver = null
        this.flow = null
        this.fswrap = fswrap
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
        if (!this.driver) {
            return webdriver.promise.fullyResolved<void>(1)
        }
        return this.driver.getSession().then(
            (session) => {
                this.driver.quit().then(
                    () => { this.driver = null; this.flow = null; })
            })
    }

    // Returns a promise which navigates the browser to another url.
    navigate_to(url: string): webdriver.promise.Promise<void> {
        return this.driver.navigate().to(url)
    }

    // Returns a promise which navigates the browser forward in the history.
    navigate_forward(): webdriver.promise.Promise<void> {
        return this.driver.navigate().forward();
    }

    // Returns a promise which navigates the browser backwards in the history.
    navigate_back(): webdriver.promise.Promise<void> {
        return this.driver.navigate().back();
    }

    // Returns a promise which refreshes the browser.
    navigate_refresh(): webdriver.promise.Promise<void> {
        return this.driver.navigate().refresh();
    }

    // Negative numbers in the iframe string path will make the path equivalent to an empty path.
    // Empty paths switch the iframe to the top iframe.
    switch_to_iframe(iframe: string): webdriver.promise.Promise<void> {
        console.log('switching to iframe: ' + iframe)

        // Switch to the top frame.
        let last_promise: webdriver.promise.Promise<void> = null
        last_promise = this.driver.switchTo().defaultContent()

        // Loop through each sub frame.
        let splits = iframe.split('/')
        for (let i=0; i<splits.length; i++) {

            // Note when empty strings are split on '/', you get an array with one element which is an empty string.
            if (splits[i] === '') {
                continue
            }

            // Get the frame index as a number.
            let frame_index = Number(splits[i])

            // Stop going deeper if get a negative index.
            if (frame_index < 0) {
                console.log('switching found -1 so going to default content')
                return last_promise
            }

            console.log('switching to frame: ' + frame_index)
            
            // Debug dump of the current iframe after switching.
            last_promise = this.driver.executeScript('return self.name').then(function (name: string){console.log('current frame name is: ' + name)})

            // Debug dump of the number of iframes.
            this.driver.findElements(By.tagName('iframe')).then(
                (iframes: webdriver.WebElement[]) => {
                    console.log('num iframes: ' + iframes.length)
                })

            // Debug dump of the number of frames.
            this.driver.findElements(By.tagName('frame')).then(
                (iframes: webdriver.WebElement[]) => {
                    console.log('num frames: ' + iframes.length)
                })
            
            // We switch to the frame by using it's WebElement instead of by its index.
            // For example as follows: this.driver.switchTo().frame(frame_index)
            // This is because otherwise it doesn't match up with the iframe index produced in PageWrap.get_iframe_path().
            this.driver.findElements(By.tagName('iframe')).then(
                (iframes: webdriver.WebElement[]) => {
                    console.log('switching to index: ' + frame_index)
                    let target_locator: any = this.driver.switchTo()
                    target_locator.frame(iframes[frame_index]);
                }) 


            // Debug dump of the name of the iframe after switching.
            last_promise = this.driver.executeScript('return self.frameElement.className + "--" + self.name + " w--" + self.frameElement.clientWidth + " h--" + self.frameElement.clientHeight')
            .then(function (name: string){console.log('frame class--name is: ' + name)})
        }
        return last_promise
    }

    // Returns a promise which resizes the browser.
    resize_browser(width: number, height: number): webdriver.promise.Promise<void> {
        return this.driver.manage().window().setSize(width, height);
    }

    // Returns a promise which jitters the browser size.
    jitter_browser_size(): webdriver.promise.Promise<void> {
        return this.driver.manage().window().getSize().then(
            (s: webdriver.ISize) => {
                this.driver.manage().window().setSize(s.width + 1, s.height + 1).then(
                    () => {
                        this.driver.manage().window().setSize(s.width, s.height);
                    }
                )
            }
        )
    }

    //------------------------------------------------------------------------------------------------
    // Dom Element Actions.
    //------------------------------------------------------------------------------------------------

    // Returns a promise which evaulates to an existing element or is rejected.
    get_element(xpath: string): webdriver.promise.Promise<webdriver.WebElement> {
        return this.driver.findElement(By.xpath(xpath))
    }
        
    // Returns a promise which evaulates to a visible element.
    // Usually we skip checking the visibility of the element because it allows us to
    // click through elements. This is very helpful when clicking video player controls
    // on web pages like cnet.
    get_visible_element(xpath: string): webdriver.promise.Promise<webdriver.WebElement> {
        return this.get_element(xpath)
            .then((element: webdriver.WebElement) => {
                return this.driver.wait(Until.elementIsVisible(element), 1000).then(
                    (found) => { return element },
                    (error) => { console.info('Warning: element was not visible: ' + xpath); throw error })
            })
    }

    // Creates promise chain which will type one key into an element.
    send_key(xpath: string, key: string): webdriver.promise.Promise<void> {
        return this.get_element(xpath).then(
            (element: webdriver.WebElement) => {
                return element.sendKeys(key)
            }
        )
    }

    get_text(xpath: string): webdriver.promise.Promise<string> {
        return this.get_element(xpath).then(
            (element: webdriver.WebElement) => {
                return element.getText()
            }
        )
    }

    // Creates promise chain which will set text on an element.
    send_text(xpath: string, text: string): webdriver.promise.Promise<void> {
        return this.get_element(xpath).then(
            (element: webdriver.WebElement) => {
                return element.sendKeys(Key.HOME, Key.chord(Key.SHIFT, Key.END), text)
            }
        )
    }

    // Creates a promise chain which will click on an element.
    click_on_element(xpath: string, relative_x: number, relative_y: number): webdriver.promise.Promise<void> {
        return this.get_element(xpath).then(
            (element: webdriver.WebElement) => {
                //return element.click()
                return this.driver.actions().mouseMove(element, { x: relative_x, y: relative_y }).click().perform()
            },
            (error: any) => {
                console.log('Error: was not able to click element.')
            }
        )
    }

    // Creates a promise chain which will mouseover an element.
    mouse_over_element(xpath: string, relative_x: number, relative_y: number): webdriver.promise.Promise<void> {
        return this.get_element(xpath).then(
            (element: webdriver.WebElement) => {
                return this.driver.actions().mouseMove(element, { x: relative_x, y: relative_y }).perform().then(
                    () => {console.log('info: moved mouse to: ' + relative_x + "," + relative_y + " over: " + xpath) },
                    (error) => { console.info('Warning: could not move_over_element (computedstyle): ' + xpath); throw (error) }
                )
            }
        )
    }

    // Creates a promise which will select an option in a select dropdown.
    select_option(xpath: string, option_text: string): webdriver.promise.Promise<void> {
        return this.get_element(xpath).then(
            (element: webdriver.WebElement) => {
                return element.findElement(By.xpath('option[normalize-space(text())="' + option_text + '"]')).click()
            }
        )
    }

    // Helper to terminate promise chains.
    static terminate_chain<T>(p: webdriver.promise.Promise<T>, id: Number) {
        p.then(
            (result) => {
                // Send the first argument in the response.
                console.log('terminiating chain with the following result:')
                try {
                    console.log("result: " + JSON.stringify(result))
                } catch(e){}
                send_msg_to_app(new ResponseMessage(id, '-1', true, result))
            },
            (error: any) => {
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
                send_msg_to_app(new ResponseMessage(id, '-1', false))
            })
    }

}