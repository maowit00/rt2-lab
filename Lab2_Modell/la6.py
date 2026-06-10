#!/usr/bin/env python3

"""
  RT2 Stuff
    - Laboraufgabe 6
"""

# %% Scaling Definition

def scale(k:float, n:int = 7):
  return int(k * 2**n) / 2**n

def scale_error(
    k:float, n:int = 7,
    verbose:bool = False
  ):

  e =  k - scale(k, n)

  if verbose: print(
    f"scaling({k} by 2^{n}) = {e:.1e}"
  )

  return e


def max_scaled_value_extension(
    k:float, n:int = 7, extended:int = 32,
    verbose:bool = False
  ):

  m = (2**(extended-1) - 1) / int(k * 2**n)

  if verbose: print(
    f"Max Betrag({k} by {n}-bits to {extended}-bits ) = {m:.2e}"
  )

  return m


# %% Labor Aufgabe

K_Rn = 1.2

scale_error(K_Rn, 7, verbose=True);

P_max = max_scaled_value_extension(K_Rn, 7, 32, verbose=True)

# %%

K_I = K_Rn * 1.536e-3 / 51.1e-3
scale_error(K_I, 14, verbose=True); # nicht viel anders

K_I = 0.036071
scale_error(K_I, 14, verbose=True);

I_max = max_scaled_value_extension(K_I, 14, 32, verbose=True)


# Zusammen

PI_max = P_max + I_max

print(f"{PI_max = :.2e}")

# %%


