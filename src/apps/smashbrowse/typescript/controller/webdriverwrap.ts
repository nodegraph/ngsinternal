import webdriver = require('selenium-webdriver')
import chrome2 = require('selenium-webdriver/chrome');
import Path2 = require('path')

import {FSWrap} from './fswrap'
import SMM = require('./socketmessage')
import CRM = require('./commrelay')
import DUM2 = require('./debugutils')


let driver: webdriver.WebDriver = null
export let Key = webdriver.Key
let By = webdriver.By
let until = webdriver.until
let flow:webdriver.promise.ControlFlow = null

export class WebDriverWrap {

    static browser_is_open(callback: (result: boolean) => void) {
        if (driver) {
            driver.getTitle().then(
                function () { callback(true) },
                function () { callback(false) });
        } else {
            callback(false)
        }
    }

    static open_browser(): void {
        try {
            // Remove the files in the chrome user data dir.
            let dir = Path2.join(FSWrap.g_user_data_dir, "chromeuserdata")
            if (FSWrap.file_exists(dir)) {
                FSWrap.delete_dir(dir)
            }
            FSWrap.create_dir(dir)

            let chromeOptions = new chrome2.Options()
            //Win_x64-389148-chrome-win32
            //Win-338428-chrome-win32
            //chromeOptions.setChromeBinaryPath('/downloaded_software/chromium/Win_x64-389148-chrome-win32/chrome-win32/chrome.exe')
            chromeOptions.addArguments("--load-extension=" + FSWrap.g_nodejs_dir + "/chromeextension")
            chromeOptions.addArguments("--ignore-certificate-errors")
            chromeOptions.addArguments("--disable-web-security")
            chromeOptions.addArguments("--user-data-dir=" + FSWrap.g_user_data_dir + "/chromeuserdata")
            chromeOptions.addArguments("--first-run")
            //chromeOptions.addArguments("--app=file:///"+url)

            // "C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" --disable-web-security --user-data-dir --app=https://www.google.com

            driver = new webdriver.Builder()
                .forBrowser('chrome')//.forBrowser('firefox')
                .setChromeOptions(chromeOptions)
                .build();

            flow = webdriver.promise.controlFlow()

            // Set default settings.
            driver.manage().timeouts().pageLoadTimeout(60000);

            webdriver.promise.controlFlow().on('uncaughtException', function (e: Error) {
                console.error('Unhandled error: ' + e);
            });
        } catch (e) {
            DUM2.DebugUtils.log_exception(e)
        }
    }

    static close_browser(): webdriver.promise.Promise<void> {
        return driver.quit()
        //driver = null
    }

    //Returns a promise which navigates the browser to another url.
    static navigate_to(url: string): webdriver.promise.Promise<void> {
        return driver.navigate().to(url)
    }

    //Returns a promise which navigates the browser forward in the history.
    static navigate_forward(): webdriver.promise.Promise<void> {
        return driver.navigate().forward();
    }

    //Returns a promise which navigates the browser backwards in the history.
    static navigate_back(): webdriver.promise.Promise<void> {
        return driver.navigate().back();
    }

    //Returns a promise which refreshes the browser.
    static navigate_refresh(): webdriver.promise.Promise<void> {
        return driver.navigate().refresh();
    }

    //Returns a promise which resizes the browser.
    static resize_browser(width: number, height: number): webdriver.promise.Promise<void> {
        return driver.manage().window().setSize(width, height);
    }

    //Returns a promise which jitters the browser size.
    static jitter_browser_size(): webdriver.promise.Promise<void> {
        return driver.manage().window().getSize().then(function (s) {
            driver.manage().window().setSize(s.width + 1, s.height + 1).then(function () {
                driver.manage().window().setSize(s.width, s.height);
            })
        })
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
    static get_element(xpath: string, wait_milli: number): webdriver.promise.Promise<{}> {
        return flow.execute(function () {
            let d = webdriver.promise.defer();
            // Wait for element to be present.
            driver.wait(function () { return driver.isElementPresent(By.xpath(xpath)); }, wait_milli)
                // Then retrieve the actual element.
                .then(function (found) {
                    if (found) {
                        driver.findElement(By.xpath(xpath)).then(
                            function (element) {
                                d.fulfill(element)
                            })
                    } else {
                        d.reject(null)
                    }
                }, function (error) {
                    console.info('Warning: could not find: ' + xpath)
                    throw error
                })
            return d.promise
        })
    }

    //Returns a promise which evaulates to a visible element.
    static get_visible_element(xpath: string, wait_milli: number): webdriver.promise.Promise<{}> {
        return WebDriverWrap.get_element(xpath, wait_milli).then(
            function (element: webdriver.WebElement) {
                return driver.wait(until.elementIsVisible(element), wait_milli).then(
                    function (element) { return element },
                    function (error) { console.info('Warning: element was not visible: ' + xpath); throw error })
            }
        )
    }

    // Creates promise chain which will type one key into an element.
    static send_key(xpath: string, key: string): void {
        let p = WebDriverWrap.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(function (element: webdriver.WebElement) {
            return element.sendKeys(key)
        })
        WebDriverWrap.terminate_chain(p)
    }

    static get_text(xpath: string) {
        let p = WebDriverWrap.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(function (element: webdriver.WebElement) {
            return element.getText()
        })
        WebDriverWrap.terminate_chain(p)
    }

    // Creates promise chain which will set text on an element.
    static send_text(xpath: string, text: string) {
        let p = WebDriverWrap.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(function (element: webdriver.WebElement) {
            return element.sendKeys(Key.HOME, Key.chord(Key.SHIFT, Key.END), text)
        })
        WebDriverWrap.terminate_chain(p)
    }

    // Creates a promise chain which will click on an element.
    static click_on_element(xpath: string) {
        console.log("clicking on element: " + xpath)
        let p = WebDriverWrap.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(function (element: webdriver.WebElement) {
            return driver.actions().click(element).perform()
        })
        WebDriverWrap.terminate_chain(p)
    }

    // Creates a promise chain which will mouseover an element.
    static mouse_over_element(xpath: string, relative_x: number, relative_y: number) {
        let p = WebDriverWrap.get_visible_element(xpath, WebDriverWrap.trivial_wait_time).then(function (element: webdriver.WebElement) {
            return driver.actions().mouseMove(element, { x: relative_x, y: relative_y }).perform().then(
                function () { },
                function (error) { console.info('Warning: could not move_over_element (computedstyle): ' + xpath); throw (error) }
            )
        })
        WebDriverWrap.terminate_chain(p)
    }

    // Creates a promise which will select an option in a select dropdown.
    static select_option(xpath: string, option_text: string) {
        let p = WebDriverWrap.get_visible_element(xpath, WebDriverWrap.critical_wait_time).then(function (element: webdriver.WebElement) {
            return element.findElement(By.xpath('option[normalize-space(text())="' + option_text + '"]')).click()
        })
        WebDriverWrap.terminate_chain(p)
    }

//// Creates a promise which will send results back to the native app.
//let send_result = function () {
//    p = flow.execute( function() {
//        let result = {test: 'replace this with real value'} // This should be changed to the real result when ready.
//        let d = webdriver.promise.defer();
//        app_socket.send('ok: ' + JSON.stringify(result))
//        d.fulfill(result)
//        return d.promise
//    })
//    terminate_chain(p)
//}

// Helper to terminate promise chains.
static terminate_chain<T>(p: webdriver.promise.Promise<T>) {
    p.then(
        function () {
            // Make sure the events are blocked. They may be unblocked to allow webdriver actions to take effect.
            let req = new SMM.RequestMessage(SMM.RequestType.kBlockEvents)
            CRM.send_message_to_extension(req)
            // Send success response to the app.
            console.log('terminating chain success with numargs: ' + arguments.length)
            if (arguments.length == 0) {
                CRM.send_message_to_app(new SMM.ResponseMessage(true))
            } else {
                // Send the first argument in the response.
                CRM.send_message_to_app(new SMM.ResponseMessage(true, arguments[0]))
            }
        },
        function (error) {
            // Make sure the events are blocked. They may be unblocked to allow webdriver actions to take effect.
            let req = new SMM.RequestMessage(SMM.RequestType.kBlockEvents)
            CRM.send_message_to_extension(req)
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
                DUM2.DebugUtils.log_exception(error)
            }
            // Send failure reponse to the app.
            CRM.send_message_to_app(new SMM.ResponseMessage(false))
        })
}

}