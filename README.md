# HW1 – Color Inversion & Histogram Equalization (Assembly + C++)

This assignment is given as part of the **BLM3061 Microprocessor Systems and Assembly Language LAB (Fall 2024-2025)** course.

## 📌 Assignment Description
You are expected to implement the following operations using **C++ + Assembly** using the four input images provided:

1. **Color Inversion**
2. **Histogram Equalization**

using **C++ + Assembly**.

## 📂 Given:
- `main.cpp` file
- 4 input images (to be used for testing)
- Visual Studio project in `hw1-visualstudio.zip`

## 🛠️ Installation and Run
1. Install **Visual Studio (x86 – 32 bit)** on your computer.
2. Extract the `hw1-visualstudio.zip` folder.

3. Open the **`hw1.vcxproj`** file within the folder in Visual Studio using `File -> Open -> Project/Solution`.

4. View the files in the Solution Explorer.

5. You only need to fill in the **assembly blocks** in the code.

> **Note:** The recommended method for Mac/Linux users is to install a Windows virtual machine and use Visual Studio. Alternatively, you can write an `asm` file and call the function with `extern C`.

## 🧩 Technical Details
- Images are stored as **unsigned char** (values ​​between `0–255`).
- You can access arrays within the assembly using **LEA + offset**.
- 32-bit registers (**EAX, EBX, ECX…**) must be used.

---

## 🔹 Question 1 – Color Inversion
- Input image: `image`
- Output image: `output_image`
- What you need to do: Apply the **inverse** operation for each pixel.

Formula:

color[i, j] = 255 – color[i, j]

---

## 🔹 Question 2 – Histogram Equalization
4-step process for histogram equalization:

1. **Histogram Creation**
- Calculate the histogram of the input image (`image`).

- Store the results in the `hist` array.

2. **Cumulative Distribution Function (CDF) Calculation**
- Create the CDF array from the histogram.

- Store it in the `cdf` array.

3. **CDF Normalization**
- Normalize the CDF values.

4. **Histogram Equalization Application**
- Calculate new pixel values ​​using the normalized CDF array.
- Write the results to the `output_image` array.

📖 More information: [Histogram Equalization – Wikipedia](https://en.wikipedia.org/wiki/Histogram_equalization)

---

## ✅ Summary
- By filling in the assembly blocks:

- You must apply the **Color Inversion** operation (single asm block)
- **Histogram Equalization** operation (4 asm blocks)
