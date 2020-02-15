smokeTest7370() {
  ./server7370 &
  echo -n Server started, press Return to continue; read
  ./client7370 localhost b0 localhost ff0000 & # RGB=ff0000 = red
  echo -n client started, press Return to continue; read
  ./client7370 localhost b0 localhost ff00 & # RGB=00ff00 = green
  echo -n client started, press Return to continue; read
  ./client7370 localhost b0 localhost ff & # RGB=0000ff = blue
  echo -n client started, press Return to continue; read
  rpcinfo -p
}

killall7370() {
  killall -q server7370 client7370
  ./deregall7370 536871065  # change this to your server num
  ./deregall7370
  rpcinfo -p
}

freshStart7370() {
  killall7370
  smokeTest7370
  ps
  rpcinfo -p
}

freshStart7370