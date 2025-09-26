# Font Setup for Label Maker

To use the text-to-G-code feature, you need to place TTF font files in this directory.

## Required Font Files

Place these TTF files in the same directory as `web-client.html`:

- `Roboto-Regular.ttf`
- `OpenSans-Regular.ttf`
- `Inter-Regular.ttf`

## Where to Download Fonts

### Google Fonts (Free)
1. Go to https://fonts.google.com/
2. Search for each font (Roboto, Open Sans, Inter)
3. Click "Download family" for each
4. Extract the TTF files from the downloaded ZIP
5. Copy the Regular weight TTF files to this directory

### Direct Download Links
- **Roboto**: https://github.com/google/roboto/releases/latest
- **Open Sans**: https://fonts.google.com/specimen/Open+Sans
- **Inter**: https://github.com/rsms/inter/releases/latest

## Running the Server

Instead of opening `web-client.html` directly in your browser, use the included Python server to avoid CORS issues:

```bash
python3 serve.py
```

Then open http://localhost:8000/web-client.html in your browser.

The server will automatically handle CORS headers needed for font loading.