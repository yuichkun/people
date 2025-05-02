import subprocess


def generateSpeech(text: str) -> None:
    """
    Generate speech from the given text using macOS 'say' command with a
    Japanese voice. The output is saved as 'tmp.aiff' in the current directory,
    overwriting if it exists.
    """
    # Use a Japanese voice (e.g., 'Kyoko' is a standard Japanese voice
    # on macOS)
    voice = "Kyoko"
    output_file = "tmp.aiff"
    try:
        subprocess.run([
            "say",
            "-v", voice,
            "-o", output_file,
            text
        ], check=True)
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Failed to generate speech: {e}")
    _convertAudio()


def _convertAudio() -> None:
    """
    Convert 'tmp.aiff' to 'tmp.wav' using ffmpeg with 8000Hz, mono, wave
    format.
    """
    input_file = "tmp.aiff"
    output_file = "tmp.wav"
    try:
        subprocess.run([
            "ffmpeg",
            "-y",  # Overwrite output file if it exists
            "-i", input_file,
            "-ar", "8000",
            "-ac", "1",
            output_file
        ], check=True)
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Failed to convert audio: {e}")


generateSpeech('王様の耳はロバの耳')