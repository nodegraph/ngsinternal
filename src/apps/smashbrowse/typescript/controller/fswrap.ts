import fs = require('fs')
import Path = require('path')

export class FSWrap {
    // This is the bin directory of the app. Nodejs is also placed there.
    static g_nodejs_dir = process.cwd()
    static g_user_data_dir = process.argv[2]  // This will be fed on the command line as node.exe bootstrap.js c:/.../some_dir


    static file_exists(path: string): boolean {
        try {
            fs.accessSync(path, fs.F_OK);
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
