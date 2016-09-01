# pisoar - PIcture SOrter for ARchaelogy
The app is suited for sorting objects on photos and making layouts from them.

# Usage
0. prepare your photos outside Pisoar - every view of object must be cropped with white border
1. create/open a database in folder with your photos
   - main database file is called pisoardb.json, do the backups
2. create database of objects interpreted by unique names
   - wrong object name can be edited (slow double click or F2) even it has views
3. select image and mark a view
   - use mouse scroll for image zoom
   - click inside the image in center of the masked object
   - Pisoar automatically creates a mask from click point to white border, check the preview
4. assign view to a object
   - mark a image file with a flags (NONE, WIP, DONE) for better readability
   - it is recommended to use shortcuts: Alt+P = assign mask to object, Alt+H = mark image as done
5. switch to Kasuar and create empty layout
6. add object on layout and customize
   - use mouse scroll for layout zoom
   - you can select one or more object (try combine mouse click with Ctrl)
   - you can move (drag and drop or arrow keys) or scale (mouse scroll with Ctrl) object views
   - use Ctrl when moving to align
7. export layout page
   - check folder "layouts" for output png files
   - or you can check folder "items" for individual views

# How to compile
On Windows you will need the qtcreator with a build packages
On Linux you will need qt libraries and qmake
