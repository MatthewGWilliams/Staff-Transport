from __future__ import unicode_literals
from .conf import settings

try:
    from south.modelsinspector import add_introspection_rules
    add_introspection_rules([], ["^.fields\.GeopositionField"])
except ImportError:
    pass
