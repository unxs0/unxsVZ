SELECT cCountryCode,cCountryName,uGeoIPCountryCode FROM tGeoIPCountryCode WHERE uGeoIPCountryCode=(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('12.12.12.12') LIMIT 1);
