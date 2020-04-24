.SUFFIXES: .java .class

.java.class:
	(cd ..; \
	javac WhiteBoard/$< )

rmic: LinesFrameImpl.class WbClientImpl.class WbServerImpl.class Invoke.class
	(cd ..; \
	rmic WhiteBoard.WbServerImpl \
		WhiteBoard.LinesFrameImpl WhiteBoard.WbClientImpl  )

WbServerImpl.class: WbServer.class
LinesFrameImpl.class: LinesFrame.class
WbClientImpl.class: WbClient.class

dox doxygen: clean
	-rm -fr ../html
	-doxygen
	-mv ../html/index.html ../html/index0.html
	cp -p ../index-jrmi.html ../html/index.html

clean:
	-rm -fr *.class *~ *_Skel* *_Stub* *.html *.bak html/ latex/
	@-echo 	killall -q -v rmiregistry java
	@-killall -q -v rmiregistry java

run:
	@echo Please source ./java7370.sh

tar:	clean
	(cd ..;	tar cvvfj 7370-WB-Java-`date +%G%m%d%H%M`.tbz WhiteBoard;\
	ls -ltr *.tbz)
