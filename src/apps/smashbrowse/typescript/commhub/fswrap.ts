import fs = require('fs')
import Path = require('path')

import {send_msg_to_app, send_msg_to_ext, get_app_server_port, get_ext_server_port} from './commhub'

export class FSWrap {

    // This is the bin dir of the install location.
    static bin_dir = process.cwd() 

    // This is the dir where settings and files are stored for the app.
    // Note this is usually somewhere completely different from the install location. 
    // This is currently fed on the command line when starting node as node.exe bootstrap.js c:/.../some_dir
    static settings_dir = process.argv[2]  

    // The prefix used in creating our chrome user data dir.
    static chrome_user_data_prefix = "chromeuserdata"
    static chrome_ext_prefix = "chromeextension"

    // Cache of the full path for our chrome user data dir.
    private chrome_user_data_dir: string

    constructor() {
        this.chrome_user_data_dir = ""
        this.create_chrome_user_data_dir()
    }

    static get_bin_dir() {
        return FSWrap.bin_dir
    }

    static get_chrome_ext_dir() {
        return Path.join(FSWrap.bin_dir + "/" + FSWrap.chrome_ext_prefix)
    }

    get_chrome_user_data_dir() {
        return this.chrome_user_data_dir
    }

    static get_smash_browse_url() {
        return FSWrap.bin_dir + '/../html/smashbrowse.html'
    }

    create_chrome_user_data_dir(): boolean {
        let i = 0
        let max = 1000 // an arbitrary max of 1000 sessions of chrome for smash browser
        while (i < max) {
            try {
                let dir = Path.join(FSWrap.settings_dir, FSWrap.chrome_user_data_prefix + i)

                // Try to remove the director if it already exists.
                // It will raise an exception if another chrome session is currently using it.
                if (FSWrap.file_exists(dir)) {
                    FSWrap.delete_dir(dir)
                }

                // Now we create a clean chrome dir for our use. 
                FSWrap.create_dir(dir)

                // If we get here we've successfully created our chrome user data dir.
                this.chrome_user_data_dir = dir
                return true
            } catch (e) {
                // Otherwise we incremented the suffix on the chrome user data dir and try again.
                i += 1
            }
        }
        return false
    }

    static copy_file(src: string, dest: string) {
        fs.createReadStream(src).pipe(fs.createWriteStream(dest));
    }

    static copy_dir(src: string, dest: string) {
        FSWrap.make_dir(dest);
        let files = fs.readdirSync(src);
        for (let i = 0; i < files.length; i++) {
            let file = files[i]
            let stats = fs.lstatSync(Path.join(src, file));
            if (stats.isDirectory()) {
                FSWrap.copy_dir(Path.join(src, file), Path.join(dest, file));
            } else if (stats.isSymbolicLink()) {
                var symlink = fs.readlinkSync(Path.join(src, file));
                fs.symlinkSync(symlink, Path.join(dest, file));
            } else {
                FSWrap.copy_file(Path.join(src, file), Path.join(dest, file));
            }
        }
    }

    static make_dir(dir: string) {
        try {
            fs.mkdirSync(dir);
        } catch (e) {
        }
    }

    static file_exists(path: string): boolean {
        try {
            fs.accessSync(path, fs.constants.F_OK);
            return true
        } catch (e) {
            return false
        }
    }

    static read_from_file(filename: string): string {
        return fs.readFileSync(filename, 'utf8')
    }

    static write_to_file(filename: string, text: string): void {
        fs.writeFileSync(filename, text, 'utf8');
    }

    static append_to_file(filename: string, text: string): void {
        fs.appendFileSync(filename, text)
    }

    static write_to_dump(text: string): void {
        FSWrap.append_to_file('./dump.txt', text)
    }

    static read_file(filename: string): string {
        let script = fs.readFileSync(filename, 'utf8')
        return script
    }

    static delete_file(filename: string): void {
        fs.unlinkSync(filename);
    }

    static delete_dir(path: string): void {
        if (fs.existsSync(path)) {
            fs.readdirSync(path).forEach(function(file) {
                let full_path = Path.join(path, file)
                if (fs.statSync(full_path).isDirectory()) {
                    FSWrap.delete_dir(full_path)
                } else {
                    fs.unlinkSync(full_path);
                }
            });
            fs.rmdirSync(path);
        }
    };

    static create_dir(path: string): void {
        if (!fs.existsSync(path)) {
            fs.mkdirSync(path);
        }
    }

}


