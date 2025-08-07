# Hacker Sky Effect (UE5 + C++)

✨ A floating text effect built in Unreal Engine 5.6 using C++. Ideal for sci-fi or hacker-themed projects.

🎥 [Watch Tutorial on YouTube](https://www.youtube.com/watch?v=lckhaGPBZKs&list=PLRaJi6uOp-KuTW_QbnODoJVioWVc2ylww&index=3)

---

## 🔧 What It Does

- Spawns floating 3D words like "ACCESS GRANTED", "WEBFLOW", "console.log"
- Makes them fall continuously and face the player camera
- Recycles words when out of bounds
- Ticks in-editor for live visual previews

---

## 📁 Files

- `AHackerSkyEffect.cpp` – actor logic
- `AHackerSkyEffect.h` – public variables + tick helpers

---

## 💡 Setup Notes

- Add your own `Text3DActorClass` blueprint
- Assign a glowing or stylized `TextMaterial`
- Toggle `bEnableEffect` in-editor to preview without Play mode

---

## 👩‍💻 Made by Crystal  
[crystalthedeveloper.ca](https://www.crystalthedeveloper.ca)
