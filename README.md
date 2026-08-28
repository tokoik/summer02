# summer02 - 今風 OpenGL の使い方（第５回 座標変換）サンプルプログラム

## 1. 概要

このプログラムは、OpenGL の **GLSL バーテックスシェーダ** と **uniform 変数** を用いて、自前で算出した投影変換行列（平行投影変換 `orthogonalMatrix` / 透視投影変換 `perspectiveMatrix`）を適用し、頂点座標をクリッピング空間へ写像して描画する手順を学ぶための、学生向けのサンプルプログラムです。本プログラムは、以下のブログ記事の解説に沿って学習を進めるための雛形として提供されています。

- [第５回 座標変換](https://tokoik.github.io/blog/今風%20opengl%20の使い方/2009/08/29/glsl.html)

OpenGL 3.0 以降で廃止された `glMatrixMode()`, `glLoadIdentity()`, `glOrtho()`, `gluPerspective()` などの固定機能行列スタックを使わず、CPU 側で行列を計算して `glUniformMatrix4fv()` でシェーダに渡す今風の手法を学習します。

## 2. 対応環境

- **Windows**: Windows 10 / 11, Visual Studio 2022 (MSVC C++17)
- **macOS**: macOS 12 Monterey 以降, Xcode 14 以降 / Command Line Tools
- **Linux**: Ubuntu 22.04 LTS 以降, GCC / Clang (C++17 対応コンパイラ)
- **ビルドツール**: CMake 3.22 以降

## 3. ビルド手順

このプログラムは [CMake](https://cmake.org/) を用いてビルド環境を整備します。各 OS とも、ソースコードが置かれているディレクトリにターミナル（またはコマンドプロンプト）で移動してから、以下の手順を実行してください。なお、プログラムをビルドするためのバイナリディレクトリは、バージョン管理ファイル（.gitignore）の設定に合わせて **build** という名前にします。

> cmake-gui で設定することも可能です。その際は、`Source code path` にはプロジェクトのフォルダを指定し、`Build path` にはプロジェクトのフォルダの中に作った build というフォルダを指定してください。その後、`Configure` → `Generate` の順にクリックした後、`Open Project` をクリックすれば、開発環境が起動します。

### 3.1 Windows (Visual Studio 2022 の場合)

1. コマンドプロンプトまたは PowerShell を開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、CMake で構成を行います。

   ```bat
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   ```

3. 生成された build フォルダ内の `summer02.sln` を Visual Studio で開きます。
4. ソリューションエクスプローラーで **summer02** プロジェクトを右クリックし、「スタートアップ プロジェクトに設定」を選択します。
5. 「ローカル Windows デバッガー」をクリックするか、F5 キーを押してビルドおよび実行します。

### 3.2 macOS (Xcode の場合)

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、Xcode 用のプロジェクトを生成します。

   ```sh
   mkdir build
   cd build
   cmake .. -G Xcode
   ```

3. 生成された `build/summer02.xcodeproj` を Xcode で開きます。
4. 左上のスキーム選択（再生ボタンの横）が **summer02** になっていることを確認します。
5. 「Run」ボタン（再生ボタン）をクリックするか、Command + R を押してビルドおよび実行します。

### 3.3 Ubuntu Linux

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 必要なパッケージ（freeglut3-dev など）がインストールされていることを確認し、以下のコマンドでビルドします。

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## 4. 起動方法

各 OS とも、ビルド後に生成されるバイナリディレクトリ (build) やそのサブフォルダから起動します。

- **Windows**

  Visual Studio 上で「ローカル Windows デバッガー」をクリックして実行するか、またはコマンドプロンプトから以下のコマンドで起動します。

  ```cmd
  cd build\Debug
  summer02.exe
  ```

- **macOS**

  Xcode 上で左上の「Run（再生ボタン）」をクリックして実行します。アプリケーションバンドルを直接起動する場合は、Finder から `build/Debug/summer02.app` を開くか、ターミナルから `open build/Debug/summer02.app` を実行します。

- **Ubuntu Linux**

  ターミナルから以下のコマンドで実行ファイル（バイナリ）を直接起動します。

  ```sh
  cd build
  ./summer02
  ```

## 5. 操作方法

- 画面右上（left = 0, right = 1, bottom = 0, top = 1）に視野空間が設定され、クリッピングされた折れ線図形が表示されます。

## 6. プログラムの解説

### 6.1 平行投影変換行列の算出

```cpp
void orthogonalMatrix(float left, float right,
  float bottom, float top,
  float zNear, float zFar,
  GLfloat* matrix)
{
  float dx = right - left;
  float dy = top - bottom;
  float dz = zFar - zNear;

  matrix[0] = 2.0f / dx;
  matrix[5] = 2.0f / dy;
  matrix[10] = -2.0f / dz;
  matrix[12] = -(right + left) / dx;
  matrix[13] = -(top + bottom) / dy;
  matrix[14] = -(zFar + zNear) / dz;
  matrix[15] = 1.0f;
  matrix[1] = matrix[2] = matrix[3] = matrix[4] =
    matrix[6] = matrix[7] = matrix[8] = matrix[9] = matrix[11] = 0.0f;
}
```

### 6.2 バーテックスシェーダでの座標変換

```glsl
// simple.vert
#version 120

invariant gl_Position;
attribute vec2 position;
uniform mat4 projectionMatrix;

void main(void)
{
  gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);
}
```
