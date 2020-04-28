# source this @file java7370.sh, as in 
# source java-env.sh
#

export CLASSPATH=.:..:WhiteBoard:~/JavaClasses # or whatever
export PATH=/home/java/bin:/usr/local/sbin:/usr/sbin:/usr/local/bin:/usr/bin:/sbin:/bin

alias pj='ps aux | grep java'		# find java* processes


wb7370clean() {
  rm -f WhiteBoard/*.class
}

wb7370build() {
  # Current dir must be the parent of .java files
  rm -f WhiteBoard/*.class
  echo javac WhiteBoard/*.java
  javac WhiteBoard/*.java
  echo not needed any more: rmic -keep WhiteBoard.LinesFrameImpl \
     WhiteBoard.WbClientImpl \
     WhiteBoard.WbServerImpl
}

wb7370rmi() {
    killall -q -v rmiregistry
    echo 'rmiregistry &'
    rmiregistry &
    echo -n sleeping for 5 sec to let rmiregistry establish itself
    sleep 5
    echo
}

deregall7370() {
  wb7370rmi
  killall -q -v java
}

wb7370run() {
    killall -q -v java

    java WhiteBoard.WbServerImpl 1 localhost &
    echo -n sleeping for 2 sec to let WbServerImpl establish itself
    sleep 2
    echo

    java WhiteBoard.WbClientImpl 22 b0 localhost //localhost/S1 FF0000 &
    java WhiteBoard.WbClientImpl  4 b0 localhost //localhost/S1 00FF00 &
    java WhiteBoard.WbClientImpl  7 b1 localhost //localhost/S1 0000FF &
    java WhiteBoard.WbClientImpl 65 b1 localhost //localhost/S1 00FFFF &
}

# -eof-

