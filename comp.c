
<!-- saved from url=(0060)http://sydney.edu.au/engineering/it/~scilect/sherlock/comp.c -->
<html><head><meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1"><script type="text/javascript" src="chrome-extension://bfbmjmiodbnnpllbbbfblcplfjjepjdn/js/injected.js"></script><style type="text/css"></style></head><body><pre style="word-wrap: break-word; white-space: pre-wrap;">/*
 *  comp.c - written by Rob Pike, modified by Loki.
 *
 *  This program doesn't quite do what the specification says.
 *  It treats all filenames on the command line which begin with '-'
 *  as options, but since the only option is '-t' any other names
 *  result in a usage message.
 */

#include &lt;stdlib.h&gt;
#include &lt;string.h&gt;
#include &lt;stdio.h&gt;

int thresh = 20;

typedef struct Sig Sig;
struct Sig
{
	int		nval;
	unsigned long	*val;
};

Sig	*load(char*);
int	compare(Sig*, Sig*);

void usage(void)
{
	fprintf(stderr, "usage: comp [-t threshold%%] sigfile othersigfile ...\n");
	fprintf(stderr, "defaults: threshold=20%%\n");
	exit(2);
}

int main(int argc, char *argv[])
{
	int i, j, nfiles, start, percent;
	char *s;
	Sig **sig;

	for (start=1; start &lt; argc; start++) {
		if (argv[start][0] != '-')
			break;	/* finished handling options */
		switch (argv[start][1]) {
		case 't':
			s = argv[++start];
			if (s == NULL)
				usage();
			thresh = atoi(s);
			if (thresh &lt; 0 || thresh &gt; 100)
				usage();
			break;
		default:
			usage();
		}
	}

	nfiles = argc - start;
	if (nfiles &lt; 2)
		usage();

	sig = malloc(nfiles * sizeof(Sig*));
	for (i=0; i &lt; nfiles; i++)
		sig[i] = load(argv[i+start]);

	for (i=0; i &lt; nfiles; i++)
		for (j=i+1; j &lt; nfiles; j++) {
			percent = compare(sig[i], sig[j]);
			if (percent &gt;= thresh)
				printf("%s and %s: %d%%\n", argv[i+start], argv[j+start], percent);
		}

	return 0;
}

/* ulcmp:  compare *p1 and *p2 */
int ulcmp(const void *p1, const void *p2)
{
	unsigned long v1, v2;

	v1 = *(unsigned long *) p1;
	v2 = *(unsigned long *) p2;
	if (v1 &lt; v2)
		return -1;
	else if (v1 == v2)
		return 0;
	else
		return 1;
}

Sig* load(char *file)
{
	FILE *f;
	int nv, na;
	unsigned long *v, x;
	char buf[512], *p;
	Sig *sig;

	f = fopen(file, "r");
	if (f == NULL) {
		fprintf(stderr, "comp: can't open %s:", file);
		perror(NULL);
		exit(2);
	}

	v = NULL;
	na = 0;
	nv = 0;
	while (fgets(buf, sizeof buf, f) != NULL) {
		p = NULL;
		x = strtoul(buf, &amp;p, 16);
		if (p==NULL || p==buf){
			fprintf(stderr, "comp: bad signature file %s\n", file);
			exit(2);
		}
		if (nv == na) {
			na += 100;
			v = realloc(v, na*sizeof(unsigned long));
		}
		v[nv++] = x;
	}
	fclose(f);

	qsort(v, nv, sizeof(v[0]), ulcmp);

	sig = malloc(sizeof(Sig));
	sig-&gt;nval = nv;
	sig-&gt;val = v;
	return sig;
}

int compare(Sig *s0, Sig *s1)
{
	int i0, i1, nboth;
	unsigned long v;

	i0 = 0;
	i1 = 0;
	nboth = 0;
	while (i0 &lt; s0-&gt;nval &amp;&amp; i1 &lt; s1-&gt;nval) {
		if (s0-&gt;val[i0] == s1-&gt;val[i1]) {
			v = s0-&gt;val[i0];
			while (i0 &lt; s0-&gt;nval &amp;&amp; v == s0-&gt;val[i0]) {
				i0++;
				nboth++;
			}
			while (i1 &lt; s1-&gt;nval &amp;&amp; v == s1-&gt;val[i1]) {
				i1++;
				nboth++;
			}
			continue;
		}
		if (s0-&gt;val[i0] &lt; s1-&gt;val[i1])
			i0++;
		else
			i1++;
	}
	return 100 * nboth / (s0-&gt;nval + s1-&gt;nval);
}

</pre></body></html>