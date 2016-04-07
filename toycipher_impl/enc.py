
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
    print "num of blocks : ", numblocks ," ; ", len(text)
    if numblocks*blocklength < len(text):
        padlength = blocklength - len(text)%blocklength
        for i in xrange(0,padlength):
            text += str('a')
        numblocks+=1
    
    print "text appended : ", text ,"  " , len(text), numblocks

    #initial block
    p0 = text[0*blocklength : (0+1)*blocklength]
    
    ci=tonum(iv)
    k0=tonum(key)
    p0=tonum(p0)
    
    ci = [(x+y+z)%25+1 for x,y,z in zip(ci, k0, p0)]
    cipher = ''.join(chr(i) for i in tonumx(ci))
    cp = ci

    # print "c0 : ", ci
    # print "p0 : ", p0
    # print "c0 : ", cp

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


def decrypt(cipher, key):
    blocklength = 10;
    iv = "qwertyuiop"
    text =""
    print "############# decryption ######################"
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

    # print "c0 : ", c0
    # print "k0 : ", k0
    # print "iv : ", iv
    # print "p0 : ", pi
    # print "p0 : ", text

    #other blocks
    for i in xrange(1,numblocks):
        #print "################### round ", i
        ci = cipher[i*blocklength : (i+1)*blocklength]
        #print "c0 txt : ", ci
        ci = tonum(ci)
        
        k0 = [(x+1)%25+1 for x in k0]
        rotate(k0, sum(cp))
        #print "c0 : ", ci
        #print "rotated key : ", k0

        pi = [ (x-y-1)%25 for x,y in zip(ci, k0)]
        txtf = ''.join(chr(i) for i in tonumx(pi))
        text += txtf
        #print "pi : ", pi
        #print "text : " , txtf
        cp = ci
        pass

    print text
    pass

if __name__ == '__main__':
    text = "a home is a dwellingplace used as a permanent or semipermanent residence for an individual, family, household or several families in a tribe. it is often a house, apartment, or other building, or alternatively a mobile home, houseboat, yurt or any other portable shelter"
    text=text.replace(".", "")
    text=text.replace(",", "")
    text = ''.join(text.split())
    #encrypt(text,"wertyuopqi")

    cipher ="plmyydcakwetffpegdupgepdhamvltvyzolpplpilaksnvkznxvkukcpdfpxpytynvlqhfhdldpvkfshzvrzlelpwmuncfxknaloybzyfzpdvsvwgayxvgxzlllymvwdlbdhiwsqcwfepakrldkvpwopgnmpmwhikencamsckqhbqfsqdhpfozsadmcqdpkoxbbadrdmgdnbezwzhbaccuwyanac"
    decrypt(cipher,"wertyuopqi")