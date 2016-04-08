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
    blocklength = 10;
    iv = "qwertyuiop"
    cipher = ""

    numblocks = len(text)/blocklength;
    #print "num of blocks : ", numblocks ," ; ", len(text)
    if numblocks*blocklength < len(text):
        padlength = blocklength - len(text)%blocklength
        for i in xrange(0,padlength):
            text += str('a')
        numblocks+=1
    
    #print "text appended : ", text ,"  " , len(text), numblocks

    #initial block
    p0 = text[0*blocklength : (0+1)*blocklength]
    
    ci=tonum(iv)
    k0=tonum(key)
    p0=tonum(p0)
    
    ci = [(x+y+z)%25+1 for x,y,z in zip(ci, k0, p0)]
    cipher = ''.join(chr(i) for i in tonumx(ci))
    cp = ci

    #other blocks
    for i in xrange(1,numblocks):
        #print "################### round ", i
        pi = text[i*blocklength : (i+1)*blocklength]
        #print "pi : ", pi
        pi = tonum(pi)
        k0 = [(x+1)%25+1 for x in k0]
        rotate(k0, sum(cp))
        #print "rotated key : ", k0
        #print "txt : ", pi

        ci = [ (x+y)%25+1 for x,y in zip(k0, pi)]
        cp = ci
        ctxt = tonumx(ci)
        ctxtf = ''.join(chr(i) for i in ctxt)
        cipher += ctxtf
        #print "ci : ", ci
        #print "cipher : " , ctxtf
        pass

    print cipher
    return cipher


def decrypt(cipher, key):
    blocklength = 10;
    iv = "qwertyuiop"
    text =""
    numblocks = len(cipher)/blocklength;
    print "num of blocks : ", numblocks ," ; ", len(cipher)
    
    if numblocks*blocklength < len(cipher):
        print "not a proper cipher , exiting ...."
        return

    #initial block
    c0 = cipher[0*blocklength : (0+1)*blocklength]
    iv = tonum(iv)
    k0 = tonum(key)
    c0 = tonum(c0)

    pi = [(x-y-z-1)%25 for x,y,z in zip(c0, k0, iv)]
    text = ''.join(chr(i) for i in tonumx(pi))
    cp = c0

    #other blocks
    for i in xrange(1,numblocks):
        #print "################### round ", i
        ci = cipher[i*blocklength : (i+1)*blocklength]
        ci = tonum(ci)    
        k0 = [(x+1)%25+1 for x in k0]
        rotate(k0, sum(cp))
    
        pi = [ (x-y-1)%25 for x,y in zip(ci, k0)]
        txtf = ''.join(chr(i) for i in tonumx(pi))
        text += txtf
        #print "text : " , txtf
        cp = ci
        pass

    print text
    return text

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
    text=text.replace(".", "")
    text=text.replace(",", "")
    text = ''.join(text.split())
    cipher = encrypt(text,"wertyuopqi")
    open('./cipher', 'w').write(cipher)

    #decrypt(cipher,"wertyuopqi")
    
    #for plotting frequencies
    plot_freq(cipher,1)
    plot_freq(cipher,2)