
# coding: utf-8

# In[13]:

from scipy import integrate
from pylab import *
import matplotlib.pyplot as plt


# In[23]:

def solve_rlc(params):
    (V,R,L,C) = params
    def rlc(A,t):
        Vc,x=A
        return array([x,(V-Vc-(x*R*C))/(L*C)])

    time = linspace(0.0,0.6e-6,1001)
    vc,x = integrate.odeint(rlc,[0.0,0.0],time).T 
    i=C*x #current 
    return (vc,i,time)

def plot_lct(params,figname):
    (vc,i,time) = solve_rlc(params)
    figure(1,figsize=(11.0,7.0))
    title("Rollno: 13d070058", fontweight='bold', fontsize=25)
    axis([0, 0.4e-6, -1, 2])
    plot(time,vc,marker='+',linestyle='-',linewidth=1.0)
    plot(time,i,marker='.',linestyle='-',linewidth=1.0)
    xlabel('t',fontweight='bold', fontsize=15) 
    ylabel('Value',fontweight='bold', fontsize=15)
    legend(['Vc','I'])
    # show()
    savefig('output/'+figname+'.png')
    clf()

# In[24]:


def plot_overdamped():
    V = 1.0 #voltageSource 
    R = 2.0 #Resistance
    L = 1.0e-9 #Inductance
    C = 100.0e-9 #Capacitance
    plot_lct((V,R,L,C),'overdamped')

def plot_criticallydamped():
    V = 1.0 #voltageSource 
    R = 2.0 #Resistance
    L = 100.0e-9 
    C = 100.0e-9 
    plot_lct((V,R,L,C),'criticallydamped')

def plot_underdamped():
    V = 1.0 #voltageSource 
    R = 2.0 #Resistance
    L = 100.0e-9 
    C = 1.0e-9 
    plot_lct((V,R,L,C),'underdamped')

def plot_specialdamped():
    V = 1.0 #voltageSource 
    R = 0 #Resistance
    L = 100.0e-9
    C = 1.0e-9
    plot_lct((V,R,L,C),'special')


# In[25]:

if __name__ == "__main__":
    plot_overdamped()
    plot_criticallydamped()
    plot_underdamped()
    plot_specialdamped()

