#!/usr/bin/python
import cgi, os
import cgitb; cgitb.enable()
form = cgi.FieldStorage()
# Get filename here.
fileitem = form['userfile']
#upload location
uploadlocation = '/tmp/www/sites/toto/'
# Test if the file was uploaded
if fileitem.filename:
   # strip leading path from file name to avoid
   # directory traversal attacks
   fn = os.path.basename(fileitem.filename)
   print("fn: ", fn)
   open(uploadlocation + fn, 'wb').write(fileitem.file.read())
   message = 'The file "' + fn + '" was uploaded successfully at '
 
else:
   message = 'No file was uploaded'
 
print """\
Content-Type: text/html\n
<html>
<body>
   <p>%s</p>
</body>
</html>
""" % (message,)
