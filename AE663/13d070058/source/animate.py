import matplotlib
import numpy as np
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.animation as manimation
from main_13d070058 import solve_rlc
import math

FFMpegWriter = manimation.writers['ffmpeg']
metadata = dict(title='Movie Test', artist='Matplotlib',
        comment='Movie support!')
writer = FFMpegWriter(fps=15, metadata=metadata)


# radar green, solid grid lines
plt.rc('grid', color='#316931', linewidth=1, linestyle='-')
plt.rc('xtick', labelsize=15)
plt.rc('ytick', labelsize=15)

# force square figure and square axes looks better for polar, IMO
width, height = matplotlib.rcParams['figure.figsize']
size = min(width, height)
# make a square figure
fig = plt.figure(figsize=(size, size))
ax = fig.add_axes([0.1, 0.1, 0.8, 0.8], polar=True, axisbg='#d5de9c')
# r = np.arange(0, 3.0, 0.01)
# theta = 2*np.pi*r
# l, =ax.plot(theta, r, color='#ee8d18', lw=3)
ax.set_rmax(2.0)
plt.grid(True)
ax.set_title("13d070058", fontsize=20)

# ploting under damped case
V = 1.0
R = 2.0
L = 100.0e-9 
C = 1.0e-9 
(vc,i,t) = solve_rlc((V,R,L,C))
t=t[:700]
alpha = R/(2*L)
w = math.sqrt(1/(L*C)-alpha*alpha)

with writer.saving(fig, "output/13d070058_anim.mp4", 100):
    for i in t:
        length = math.exp(-1*alpha*(i))
        arr2 = plt.arrow(w*i, 0, 0, (length)*2*0.8, alpha = 0.5, width = 0.015, head_width=0.1, head_length=(length)*2*0.2,
                 edgecolor = 'black', facecolor = 'red', lw = 2, zorder = 5)
        writer.grab_frame()
        arr2.remove()
