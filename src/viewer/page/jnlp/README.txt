launching VizStreams using java web start
01/06/07

When the application needs to request all permissions, the
following can be done:

1) Add:
  <security>
      <all-permissions/>
  </security>
to the .jnlp file.

2) keytool -genkey -keystore myKeys -alias carlos
# password: 123456

3) jarsigner -keystore myKeys lib/geostreams.jar carlos

References:
	http://jan.netcomp.monash.edu.au/java/jini/tutorial/JNLP.xml
	http://java.sun.com/developer/technicalArticles/Programming/jnlp
