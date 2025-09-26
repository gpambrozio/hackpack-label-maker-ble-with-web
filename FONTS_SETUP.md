# Font Setup for Label Maker

The text-to-G-code feature now supports both **Google Fonts** (loaded automatically) and **local TTF files**.

## 🆕 Google Fonts Support (NEW!)

The label maker now includes **11 additional Google Fonts** that load automatically:

- Lato
- Montserrat
- Oswald
- Source Sans Pro
- Raleway
- Ubuntu
- Nunito
- Playfair Display
- Merriweather
- Dancing Script
- Pacifico

**Note:** Roboto, Open Sans, and Inter are available as local fonts (faster loading) and are not duplicated in the Google Fonts list.

**No setup required!** These fonts load directly from Google Fonts when you select them.

## Local Font Files (Optional)

You can also use local TTF files for faster loading or offline use:

### Required Files (Optional)
Place these TTF files in the same directory as `web-client.html`:

- `Roboto-Regular.ttf`
- `OpenSans-Regular.ttf`
- `Inter-Regular.ttf`

### Where to Download Local Fonts

#### Google Fonts (Free)
1. Go to <https://fonts.google.com/>
2. Search for each font (Roboto, Open Sans, Inter)
3. Click "Download family" for each
4. Extract the TTF files from the downloaded ZIP
5. Copy the Regular weight TTF files to this directory

#### Direct Download Links
- **Roboto**: <https://github.com/google/roboto/releases/latest>
- **Open Sans**: <https://fonts.google.com/specimen/Open+Sans>
- **Inter**: <https://github.com/rsms/inter/releases/latest>

## Running the Server

Instead of opening `web-client.html` directly in your browser, use the included Python server:

```bash
python3 serve.py
```

Then open <http://localhost:8000/web-client.html> in your browser.

The server handles CORS headers needed for both Google Fonts and local font loading.

## How It Works

The font system uses a **3-tier fallback strategy with local fonts prioritized**:

1. **Local Fonts**: Fastest loading from local TTF files (preferred when available)
2. **Google Fonts**: Loads WOFF2 fonts directly from Google's official CDN via CORS proxy
3. **Simple Fonts**: Creates basic geometric fonts as final fallback

**CORS Solution:** Uses cors-anywhere proxy to bypass CORS restrictions when accessing Google Fonts API from localhost.

Fonts are cached after first load for better performance.