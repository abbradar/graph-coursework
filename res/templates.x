xof 0303txt 0064

template Frame
{
    < 3D82AB46-62DA-11CF-AB39-0020AF71E433 >
    [...]
}

template MeshFace
{
    < 3D82AB5F-62DA-11cf-AB39-0020AF71E433 >
    dword nFaceVertexIndices;
    array dword faceVertexIndices[nFaceVertexIndices];
}

template Vector
{
    < 3D82AB5E-62DA-11cf-AB39-0020AF71E433 >
    float x;
    float y;
    float z;
}

template Mesh
{
    <3D82AB44-62DA-11CF-AB39-0020AF71E433>
    dword nVertices;
    array Vector vertices[nVertices];
    dword nFaces;
    array MeshFace faces[nFaces];
    [...]
}

template MeshNormals
{
    < F6F23F43-7686-11cf-8F52-0040333594A3 >
    dword nNormals;
    array Vector normals[nNormals];
    dword nFaceNormals;
    array MeshFace faceNormals[nFaceNormals];
}
