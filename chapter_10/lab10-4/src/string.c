
typedef unsigned int size_t;

size_t strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		;

	return sc - s;
}

void *memcpy(void *dest, const void *src, size_t count)
{
	char *tmp = dest;
	const char *s = src;

	while (count--)
		*tmp++ = *s++;
	return dest;
}
