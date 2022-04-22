import cgi, cgitb               
      
your_name = "lucas"   
 
print ("<html>") 
print ("<head>") 
print ("<title>First CGI Program</title>") 
print ("</head>") 
print ("<body>") 
print ("<h2>Hello, %s is me</h2>" % (your_name)) 
  
print ("</body>") 
print ("</html>")
