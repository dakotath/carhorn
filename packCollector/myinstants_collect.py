import requests
from bs4 import BeautifulSoup
import os
import re

from pydub import AudioSegment
import wave
import struct
import sys
import json

url = 'https://www.myinstants.com/en/index/ca/'

# Remove old downloads
try:
    os.rmdir("downloads")
except Exception as e:
    print(f"Oh well, sh*t happens: {e}")

# Make downloads folder
try:
    os.mkdir("downloads")
except Exception as e:
    print(f"Oh well, sh*t happens: {e}")

response = requests.get(url)
soup = BeautifulSoup(response.content, 'html.parser')

sounds = []
finalMeta = []

def download_audio(url, filename, folder):
    response = requests.get(url)
    if response.status_code == 200:
        with open(os.path.join(folder, filename), 'wb') as f:
            f.write(response.content)
        print(f"Downloaded {filename}")
    else:
        print(f"Failed to download {filename}")

def convert_to_pcm_8bit_signed(input_file, output_file):
    # Load audio file using pydub
    sound = AudioSegment.from_file(input_file)

    # Convert audio to 8-bit PCM
    sound = sound.set_sample_width(1)
    sound = sound.set_frame_rate(32000)  # Change the frame rate to 32000 Hz
    sound = sound.set_channels(1)  # Ensure mono audio (1 channel)

    # Export as raw PCM data
    raw_data = sound.raw_data

    # Write raw PCM data to a .wav file with PCM 8-bit signed format
    with wave.open(output_file, 'wb') as output_wav:
        output_wav.setnchannels(1)  # Mono
        output_wav.setsampwidth(1)  # 8-bit
        output_wav.setframerate(32000)  # Set the frame rate to 32000 Hz
        output_wav.writeframes(raw_data)
    
    # Remove source file.
    os.remove(input_file)

for instant in soup.find_all('div', class_='instant'):
    # Extracting color code
    circle_div = instant.find('div', class_='circle small-button-background')
    color_code = circle_div['style'].split(':')[1].strip(' ;')

    # Extracting sound title
    title_a = instant.find('a', class_='instant-link link-secondary')
    sound_title = title_a.text.strip()

    # Extracting sound URL
    onclick_attr = instant.find('button', class_='small-button')['onclick']
    sound_url = onclick_attr.split("'")[1]

    sounds.append({
        'title': sound_title,
        'url': "https://myinstants.com" + sound_url,
        'color': color_code
    })

# Print all extracted sound data
for sound in sounds:
    print(f"Title: {sound['title']}")
    print(f"URL: {sound['url']}")

    sndSplit = sound['url'].split('.')
    sndType = sndSplit[len(sndSplit)-1]
    sndFileSplt = sound['url'].split('/')
    sndFile = sndFileSplt[len(sndFileSplt)-1]
    
    print(f"format: {sndType}")
    download_audio(sound['url'], sndFile, "downloads")
    sndFileRepl = sndFile.replace("mp3", "pcm")
    convert_to_pcm_8bit_signed(f"downloads/{sndFile}", f"downloads/{sndFileRepl}")

    newEntry = {
        "file": f"{sndFileRepl}",
        "name": f"{sound['title']}",
        "color": f"{sound['color']}"
    }

    finalMeta.append(newEntry)
    
    print(f"Color Code: {sound['color']}")
    print()

print(json.dumps(finalMeta))

with open("downloads/meta.json", "w") as mjd:
    mjd.write(json.dumps(finalMeta))