#! bin/python
from collections import Counter
import matplotlib.pyplot as plt

def tonum(input):
        output = []
        for character in input.lower():
            number = ord(character) - 96;
            if number > 9:
                number = number-1;
            output.append(number);
        return output

def tonumx(input):
        output = []
        for number in input:
            if number > 9:
                number = number+1;
            number = number + 96;
            output.append(number);
        return output

def rotate(l, x, d='f'):
    if d == 'f':
        return l[-x % len(l):] + l[:-x % len(l)]
    elif d == 'b':
        return l[x % len(l):] + l[:x % len(l)]
    else:
        return False

def encrypt(text, key):
    blocklength = len(key);
    cipher = ""
    print "blocklength : ", blocklength

    numblocks = len(text)/blocklength;
    #print "num of blocks : ", numblocks ," ; ", len(text)
    if numblocks*blocklength < len(text):
        padlength = blocklength - len(text)%blocklength
        for i in xrange(0,padlength):
            text += str('a')
        numblocks+=1

    cis=[]
    #initial block
    k0=tonum(key)
    #intial block
    for i in xrange(0,numblocks):
        #print "################### round ", i
        pi = text[i*blocklength : (i+1)*blocklength]
        pi = tonum(pi)
        ci = [(x+y)%25+1 for x,y in zip(k0,pi)]
        cis.append(ci)
        rotate(ci, i)
        k0 = [ (x+y)%25+1 for x,y in zip(k0, ci)]
        pass
    
    cipher = ""
    tmp = []
    # permutation
    for i in xrange(0,blocklength):
        for j in xrange(0,len(cis)):
            tmp.append(cis[j][i])
        ci = ''.join(chr(i) for i in tonumx(tmp))
        cipher += ci
        tmp = []
    print cipher
    return cipher

def plot_freq(text, n):
    if n==2:
        c1 = Counter( [text[i:i+n] for i in range(0, len(text), n)] )
        text = text[1:]
        c2 = Counter( [text[i:i+n] for i in range(0, len(text), n)] )

        c1.update(c2)
    elif n==1:
        c1 = Counter(text)
        tot = len(text)
    else:
        return
    #print c1
    print c1.most_common(10)
    tot = len(c1)
    c1 = { key: value/float(tot) for key, value in c1.iteritems() }
    plt.bar(range(len(c1)), c1.values(), align='center')
    plt.xticks(range(len(c1)), c1.keys())
    plt.show()

if __name__ == '__main__':
    filename = "./text"
    text = open(filename, 'r').read()
    ntxt = ""
    text = ''.join(text.split())
    for x in text:
        if str(x).isalpha():
            ntxt += str(x)
            pass
    text = ntxt
    #cipher = encrypt(text,"wertyuopqi")
    #open('./cipher', 'w').write(cipher)

    #for plotting frequencies
    # plot_freq(cipher,1)
    # plot_freq(cipher,2)