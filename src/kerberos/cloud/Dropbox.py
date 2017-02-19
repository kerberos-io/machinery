import datetime
import ntpath
import os
from dropbox import dropbox


# authorize dropbox
def authorize(token):
    session = dropbox.Dropbox(token)
    session.users_get_current_account()
    return session


# upload frame to dropbox
def upload(session, folder, pathToImage):
    modification_time = os.path.getmtime(pathToImage)
    path = '/%s/%s/%s' % (folder, folder, ntpath.basename(pathToImage))
    try:
        with open(pathToImage, "rb") as f:
            data = f.read()
            res = session.files_upload(data, path, dropbox.files.WriteMode.add,
                                       client_modified=datetime.datetime(*datetime.time.gmtime(modification_time)[:6]),
                                       mute=True)
        return True
    except Exception as ex:
        return False
