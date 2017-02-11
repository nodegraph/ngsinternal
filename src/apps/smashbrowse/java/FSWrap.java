import java.io.File;

public class FSWrap {
	
	private static final String chrome_user_data_prefix = "chromeuserdata";
	
	public static boolean destory_file(File file) {
		if (file.isDirectory()) {
			File[] children = file.listFiles();
			for (int i = 0; i < children.length; i++) {
				boolean success = destory_file(children[i]);
				if (!success) {
					return false;
				}
			}
		}
		return file.delete();
	}
	
	public static String create_chrome_user_data_dir(String settings_dir) {
        int i = 0;
        int max = 1000; // an arbitrary max of 1000 sessions of chrome for smash browser
        while (i < max) {
            try {
                String path = settings_dir + java.io.File.separator + chrome_user_data_prefix + Integer.toString(i);
                File dir = new File(path);
                
                // Try to remove the director if it already exists.
                // It will raise an exception if another chrome session is currently using it.
                if (dir.exists()) {
                	System.err.println("attempt to destroy dir: " + dir);
                    if (!FSWrap.destory_file(dir)) {
                    	System.err.println("error could not destroy dir: " + dir);
                    	i += 1;
                    	continue;
                    }
                }

                // Now we create a clean chrome dir for our use. 
                dir.mkdir();

                // If we get here we've successfully created our chrome user data dir.
                return dir.toString();
            } catch (Exception e) {
                // Otherwise we incremented the suffix on the chrome user data dir and try again.
                i += 1;
            }
        }
        return "";
    }
	
	public static boolean platform_is_windows() {
		String platform = System.getProperty("os.name");
		if (platform.startsWith("Windows")) {
			return true;
		} else {
			return true;
		}
	}
	
}
