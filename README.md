# OCR Word Search Solver

An OCR-based application that automatically solves hidden word search puzzles using neural network character recognition.

## Features

- **Neural Network Training**: Train the OCR model to recognize letters with multi-threaded processing
- **Image Processing**: Load and preprocess word search puzzle images
- **Character Recognition**: Automatically detect and recognize letters in the grid
- **Puzzle Solving**: Find and highlight all hidden words in the puzzle
- **Image Export**: Save the solved puzzle with highlighted solutions

## Prerequisites

- GCC compiler
- GTK+ 3.0 development libraries
- GdkPixbuf library
- Make

### Installation (Debian/Ubuntu)

```bash
sudo apt-get install build-essential libgtk-3-dev libgdk-pixbuf2.0-dev
```

## Building the Application

1. Navigate to the GUI directory:
```bash
cd src/GUI
```

2. Compile the application:
```bash
make
```

## Usage

1. Run the application:
```bash
./ocr_gui
```

2. **Train the Neural Network** (first-time setup):
   - Click the "Train OCR" button
   - Wait for the training process to complete
   - Training runs in a separate thread to keep the UI responsive

3. **Load a Word Search Image**:
   - Click "Load Image"
   - Select your word search puzzle image file

4. **Solve the Puzzle**:
   - Click "Solve"
   - The application will:
     - Preprocess the image
     - Detect and extract character blocks
     - Recognize all letters using the neural network
     - Find all hidden words in the grid
     - Draw red lines over found words

5. **Save the Solution**:
   - Click "Save"
   - Choose the destination path
   - The solved puzzle image will be saved with highlighted words

## Additional Features

- **Manual Rotation**: Enter an angle and click "Rotate" to manually adjust image orientation
- **Real-time Status**: Monitor processing progress in the status bar

## Project Structure

```
src/
├── GUI/              # Graphical user interface
├── ocr/              # Neural network and OCR logic
├── pretreatment/     # Image preprocessing
├── solver/           # Word search solving algorithm
└── matrix/           # Matrix operations for neural network
```

## Credits
Alexendre Abbruzzese
Benjamin Poussart
Lilian Cogan
Oscar Cornut

## License

This project is for educational purposes.
