from django.core.management.base import BaseCommand
from RouteFinder.models import Request

class Command(BaseCommand):
    def handle(self, *args, **options):
        Request.objects.all().delete()
