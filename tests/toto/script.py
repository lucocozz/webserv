import cgi, cgitb               
import os

your_name = "42webserv"   
 
print ("<html>") 
print ("<head>") 
print ("<title>First CGI Program</title>") 
print ("</head>") 
print ("<body>") 
print ("<h2>Hello, Welcome to %s</h2>" % (your_name)) 
  
print("Meta-variables:")
for evar in os.environ.keys():
    print("<b> %20s </b> :%s" % (evar, os.environ[evar]))
    print()

print ("</body>") 
print ("</html>")
