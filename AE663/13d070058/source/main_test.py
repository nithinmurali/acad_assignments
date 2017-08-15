
from scipy.integrate import odeint
import numpy as np
import unittest
import main_13d070058 as mi


class TestLCTank(unittest.TestCase):
    
    def test_overdamped_vc(self):
        V = 1.0 #voltageSource 
        R = 2.0 #Resistance
        L = 1.0e-9 #Inductance
        C = 100.0e-9 #Capacitance
        (vc,i,t) = mi.solve_rlc((V,R,L,C))
        self.assertNotEqual(i[len(vc)-1],0)

    def test_underdamped(self):
        V = 1.0 #voltageSource 
        R = 2.0 #Resistance
        L = 100.0e-9 #Inductance
        C = 1.0e-9 #Capacitance
        (vc,i,t) = mi.solve_rlc((V,R,L,C))
        self.assertNotEqual(vc[10],0)

if __name__ == '__main__':
    unittest.main()