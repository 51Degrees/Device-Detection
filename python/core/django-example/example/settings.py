import sys

DEBUG = True

DATABASES = {}

ROOT_URLCONF = 'example.urls'

WSGI_APPLICATION = 'example.wsgi.application'

SECRET_KEY = 'd3m0'

SESSION_ENGINE = 'django.contrib.sessions.backends.cache'

SESSION_COOKIE_NAME = 'sid'

SESSION_SAVE_EVERY_REQUEST = True

CACHES = {
    'default': {
        'BACKEND': 'django.core.cache.backends.locmem.LocMemCache',
        'LOCATION': 'example'
    }
}

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'fiftyone_degrees.mobile_detector.contrib.django.middleware.DetectorMiddleware',
)

INSTALLED_APPS = (
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'example',
)

TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        'DIRS': ['example/templates'],
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.i18n',
                'django.template.context_processors.request',
                'fiftyone_degrees.mobile_detector.contrib.django.context_processors.device',
            ],
            'loaders': [
                'django.template.loaders.app_directories.Loader',
            ]
        },
    },
]

LOGGING = {
    'version': 1,
    'disable_existing_loggers': False,
    'formatters': {
        'simple': {
            'format': '%(levelname)s %(message)s'
        },
    },
    'handlers': {
        'console': {
            'level': 'DEBUG',
            'class': 'logging.StreamHandler',
            'formatter': 'simple',
            'stream': sys.stdout,
        },
    },
    'loggers': {
        '': {
            'handlers': ['console'],
            'level': 'DEBUG',
            'propagate': True,
        },
    },
}


FIFTYONE_DEGREES_MOBILE_DETECTOR_SESSION_CACHE = True

FIFTYONE_DEGREES_MOBILE_DETECTOR_SESSION_FIELD = '_51degrees_device'
