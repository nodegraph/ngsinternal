# .bashrc

THIS_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Uncomment this to see the actual bash script which sets up the environment.
#echo "$(python ${THIS_SCRIPT_DIR}/openshapesenv.py)"

eval "$(python ${THIS_SCRIPT_DIR}/android.py)"
