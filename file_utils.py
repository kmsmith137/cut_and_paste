import os
import errno


def makedirs(name):
    """A version of os.makedirs() which does not raise an exception if the directory already exists."""

    # Ensures correct handling of
    #   makedirs(os.path.dirname(f))
    # in the case where f is just an ordinary filename with no slashes

    if name == '':
        return

    try:
        os.makedirs(name)
    except OSError, exc:
        if (exc.errno != errno.EEXIST) or not os.path.isdir(name):
            raise
