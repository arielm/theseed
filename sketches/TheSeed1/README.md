# The seed :: Issue 1

The [first edition](https://www.fxhash.xyz/generative/18334) of our fx(hash) zine.

Instructions for building WASM version on macOS:

1. Depends on [chronotext-cross](https://github.com/arielm/chronotext-cross) for infrastructure.
   - Clone with:
   ```
   git clone --recurse-submodules https://github.com/arielm/chronotext-cross
   ```
   
   - To be done each time Terminal is started (will set the CROSS_PATH variable):
   ```
   cd path/to/chronotext-cross
   source setup.sh
   ```
   
   - [Install emscripten](https://github.com/arielm/chronotext-cross/wiki/Installing-Emscripten-on-macOS)

2. Clone this repository with:
   ```
   git clone https://github.com/arielm/theseed
   ```

3. Add THESEED_PATH variable:
   ```
   export THESEED_PATH=path/to/theseed
   ```

4. Build and run:
   ```
   cd $THESEED_PATH/sketches/TheSeed1
   RUN_TEST -DPLATFORM=emscripten
   ```
   
   The resulting WASM files will be in `build/emscripten`. Note that it won't properly run, because `TheSeed1.html` doesn't include the fx(hash) snippet.
