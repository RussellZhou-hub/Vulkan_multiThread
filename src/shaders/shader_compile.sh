glslangValidator shader.vert -V -o vertex.spv
glslangValidator shader.frag -V -o fragment.spv
cp -r ./vertex.spv ../../bin//DebugEditor/shaders/
cp -r ./fragment.spv ../../bin//DebugEditor/shaders/
rm *.spv