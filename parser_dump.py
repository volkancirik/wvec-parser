#!/usr/bin/python
import sys,random
usage = """USAGE\n_____________________________
python parser_dump.py file0 contextL embDim maxS

file0    : CONLL formatted file with word embeddings in 11th columnt
contextL : add neighboring word embeddings of target word, for instance contexL=2  -2,-1,0(target),+1,+2 are combined
embDim   : dimension of word embeddings
maxS     : maximum #of sentences
"""

def concatContext(s,e,clen,CL,L,SL):
	dummyBeg = L*[0]
	dummyEnd = L*[0]
	for i in xrange(clen):
		e.insert(0,dummyBeg)
		e.append(dummyEnd)
	T = []
	for j in xrange(SL):
		t = []
		for i in xrange(CL):
			t+= e[j+i]
		T.append(t)
	return T

def readConll(inFile,clen,L,maxS):
	try:
		inf = open(inFile)
	except:
		print "cannot open conll file"
		quit()
	CL = clen*2+1

	sentences = []
	s = []
	h = []
	e = []
	sid = 0

	for line in inf:
		l = line.strip().split("\t")
		if len(l) < 11:
			if len(s) == 0:
				break
			SL = len(s)
			T = concatContext(s,e,clen,CL,L,SL)
			visible = {}

			for x in xrange(SL):
				visible[x] = T[x]

			sentences.append((s,h,visible))
			s = []
			h = []
			e = []
			sid +=1
			if sid % 100 == 0:
				print >> sys.stderr, sid,"sentences read & prepared from",inFile,"target ",maxS
			if sid >= maxS:
				break;
			continue
		s.append(l[1])
		h.append(int(l[6]))
		e.append([float(emb) for emb in l[10].split()])
	return sentences

def dump(dataset,VL):

	output = []
	for (s,h,visible) in dataset:
		output.append((1,VL*[1]))
		SL = len(s)
		if SL == 1:
			continue
		for i in xrange(SL):
			hgold = h[i]+1
			output.append((hgold,visible[i]))
		output.append((-2,[]))
	for label,feats in output:
		if label == -2:
			print "-2"
			continue
		print label,
		for i,f in enumerate(feats):
			print " ".join([str(f)]),
		print
if len(sys.argv)!=5:
	print >> sys.stderr, usage
	quit(1)

clen= int(sys.argv[2])
L = int(sys.argv[3])
maxs= int(sys.argv[4])
data = readConll(sys.argv[1],clen,L,maxs)
dump(data,L*(2*clen+1))
