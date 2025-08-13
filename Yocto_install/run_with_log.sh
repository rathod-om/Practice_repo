mkdir -p logs
script -q -f "logs/$(date +%H_%M_%S_%d_%m_%y)_LOG.txt" -c "./Yocto_install_script.sh"
