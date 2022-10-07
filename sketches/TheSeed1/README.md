# The seed :: Issue 1

The [first edition](https://www.fxhash.xyz/generative/18334) of our fx(hash) zine.

### Instructions for building WASM + WebGL version on macOS:

1. Install Emscripten and build the chronotext-cross library following [these instructions](https://arielm.github.io/cross-blog/2022/10/06/running-in-the-browser.html)

2. Clone this repository with:
   ```
   git clone https://github.com/arielm/theseed
   ```

3. Add `THESEED_PATH` variable:
   ```
   export THESEED_PATH=path/to/theseed
   ```

4. Build and run:
   ```
   cd $THESEED_PATH/sketches/TheSeed1
   RUN_TEST -DPLATFORM=emscripten
   ```
   
   The resulting `.wasm`, `.js` and `.data` files will be in `build/emscripten`. Note that it won't properly run, because `TheSeed1.html` doesn't include the fx(hash) snippet. You will have to add it yourself and run again `TheSeed1.html` on a server.
