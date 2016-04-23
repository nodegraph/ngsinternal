import os
import sys

SCRIPT_FILE =os.path.realpath(__file__)
SCRIPT_DIR = os.path.dirname(SCRIPT_FILE)

# Helper method to get iteritems from a dict. This is compatible with both python 2 and 3.
def get_iter_items(d):
    if (sys.version_info > (3, 0)):
        # Python 3 code in this block
        return d.items()
    # Python 2 code in this block
    return d.iteritems()

# The Env manages a number of search paths. Currently 5 of them.
# It's primary functionality is to translate these out into environment variables in a bash script.
# In particular the search paths are translated out into compiler and runtime environment variables.
# The the same search path can be used for different env variables. For example LDFLAGS AND LD_LIBRARY_PATH both use the LIB search paths
# The final bash script is dumped to the shell.
# This is picked up by a parent bash script which will eval the python output to setup the environment properly.
# As an aside this class also keeps track of extra aliases and environment variables that need to be set in the environment.
# Settings for a new environment should typically subclass another Env class.
class Env(object):

    # Enums for the types of search paths that we manage.
    BIN=0
    INC=1
    LIB=2
    PKG=3
    PYT=4

    # The env variables that we can traslate the above search paths out to. 
    compiler_path_vars={INC:"CPPFLAGS", LIB:"LDFLAGS"}
    runtime_path_vars={BIN:"PATH", LIB:"LD_LIBRARY_PATH", PKG:"PKG_CONFIG_PATH", PYT:"PYTHONPATH"}

    # Some env variables are just copies of other ones.
    mirrored_vars={'CXXFLAGS':'CPPFLAGS'}

    # Extra env variables that we should wipe out, to keep our environment safe from external effects.
    wiped_vars=['CFLAGS']

    def __init__(self):
        self.__reset()
        # Methods should fill in the following primary member variables.
        # self.search_paths
        # self.aliases
        # self.vars
        # This class will then handle translated these out into the bash script.

    def get_script(self):
        # Reset everything.
        self.__reset()
        # Initialization/Setup. These are the methods should be overriden in derived classes.
        self.setup_aliases()
        self.setup_vars()
        self.setup_compiler()
        self.setup_paths()
        # With the above settings, we can now compute and inject more vars into self.vars.
        self.__compute_compiler_flags()
        self.__compute_runtime_paths()
        # Bash script creation.
        self.__set_aliases()        # sets all aliases
        self.__set_vars()           # sets all vars, include those computed just above
        self.__set_mirrored_vars()  # copies env var values to other env vars
        # Dump out to the shell.
        for line in self.script:
            print(line)       

    def __reset(self):
        # Our search paths.
        self.search_paths={}
        self.search_paths[Env.BIN]=[]
        self.search_paths[Env.INC]=[]
        self.search_paths[Env.LIB]=[]
        self.search_paths[Env.PKG]=[]
        self.search_paths[Env.PYT]=[]
        # Our aliases.
        self.aliases={}
        # Our other vars.
        self.vars={}
        # Our output bash script.
        self.script=[]

    def __compute_path_list(self, path_list, prefix=":"):
        paths=""
        for path in path_list[::-1]:
            paths+=prefix
            paths+=path
        return paths

    # --------------------------------------------------------------------
    # Methods which compute env vars related to compiler flags. (eg CPPFLAGS, CXXFLAGS, LDFLAGS)
    # These should not be overriden.
    # --------------------------------------------------------------------
    def __compute_compiler_flags(self):

        for k,v in get_iter_items(self.compiler_path_vars):
            if k == Env.INC:
                path_list = self.__compute_path_list(self.search_paths[k]," -I")
                self.vars[v]='"%s ${%s}"' % (path_list,v)
            elif k == Env.LIB:
                path_list = self.__compute_path_list(self.search_paths[k]," -L")
                self.vars[v]='"%s ${%s}"' % (path_list,v)

    # --------------------------------------------------------------------
    # Methods which compute env vars related to runtime vars. (eg PATH, LD_LIBRARY_PATH)
    # These should not be overriden.
    # --------------------------------------------------------------------
    def __compute_runtime_paths(self):
        for k,v in get_iter_items(self.runtime_path_vars):
            path_list = self.__compute_path_list(self.search_paths[k],":")
            self.vars[v]='"%s:${%s}"' % (path_list,v)

    # --------------------------------------------------------------------
    # Methods which output to the internal bash script.
    # These should not be overriden.
    # --------------------------------------------------------------------

    # Aliases.
    def __set_alias(self, key, value):
        self.script.append("alias %s='%s'" % (key,value) )
    def __set_aliases(self):
        self.script.append("######################################################################################################")
        self.script.append("## Setting Aliases ")
        self.script.append("######################################################################################################")
        for k,v in get_iter_items(self.aliases):
            self.__set_alias(k,v)

    # Environment variables.
    def __set_var(self, var, value):
        self.script.append("######################################################################################################")
        self.script.append("## Setting Var: %s" % var)
        self.script.append("######################################################################################################")
        self.script.append("unset %s" % var) 
        self.script.append('export %s=""' % var)
        self.script.append('%s=%s' % (var, value))
    def __set_vars(self):
        for k,v in get_iter_items(self.vars):
            self.__set_var(k,v)
    
    # Mirrored variables. 
    # This should be called at the very when all the others vars have been computed.
    def __set_mirrored_vars(self):
        for k,v in get_iter_items(self.mirrored_vars):
            self.__set_var(k,"${"+v+"}")

    # --------------------------------------------------------------------
    # Initialization and Setup.
    # --------------------------------------------------------------------

    # Aliases.
    def setup_aliases(self):
        # Quick path aliaes.
        #self.aliases['rm']='rm -i'
        pass

    def setup_vars(self):
        pass

    # Compiler vars.
    def setup_compiler(self):
        # Setup vars related to compilers.
        #self.vars['CXX']='g++'
        #self.vars['CC']='gcc'
        #self.vars['LD']='g++'
        # Setup aliases related to compilers.
        pass

    # Path setup.
    def add_path(self, path_type, path):
        self.search_paths[path_type].append(path)

    # Path setup.
    def setup_paths(self):
        # cent os system executables.
        self.add_path(Env.BIN,"/usr/local/sys/bin:/usr/local/bin:/bin:/usr/bin:/usr/X11R6/bin")

