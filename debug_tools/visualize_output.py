
import os
import matplotlib.pyplot as plt
import struct
import numpy as np
from scipy.signal import stft

def plot_curve_and_stft(file_path, fs=48000):
    # Lecture du fichier binaire
    with open(file_path, 'rb') as file:
        # Lire les données en format binaire (float)
        data = []
        while True:
            binary_data = file.read(4)
            if not binary_data:
                break
            value = struct.unpack('f', binary_data)[0]
            data.append(value)

    # Création de la courbe
    plt.figure(figsize=(12, 4))
    plt.subplot(1, 2, 1)
    plt.plot(data)
    plt.title('Courbe à partir du fichier binaire')
    plt.xlabel('Indice')
    plt.ylabel('Valeur')

    # Calcul de la STFT
    f, t, Zxx = stft(data, fs=fs, nperseg=256)
    
    # Affichage de la STFT
    plt.subplot(1, 2, 2)
    plt.pcolormesh(t, f, np.abs(Zxx), shading='gouraud')
    plt.title('STFT')
    plt.ylabel('Fréquence [Hz]')
    plt.xlabel('Temps [s]')
    plt.colorbar(label='Amplitude')

    # Affichage des deux sous-plots
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: python script.py chemin_vers_votre_fichier.bin")
        sys.exit(1)

    file_path = sys.argv[1]

    if not os.path.exists(file_path):
        print(f"Le fichier {file_path} n'existe pas.")
        sys.exit(1)

    plot_curve_and_stft(file_path)
