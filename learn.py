import numpy as np
import pandas as pd
from sklearn.linear_model import LogisticRegression
from sklearn.preprocessing import LabelEncoder
from sklearn.model_selection import train_test_split

data = pd.read_csv('all.csv')

data = data.sample(frac=1).reset_index(drop=True)

X = data[['rf', 'gf', 'bf']].values
y = data['category'].values

label_encoder = LabelEncoder()
y_encoded = label_encoder.fit_transform(y)
class_names = label_encoder.classes_

X_train, X_test, y_train, y_test = train_test_split(X, y_encoded, test_size=0.2, random_state=42)

model = LogisticRegression(multi_class='multinomial', solver='lbfgs', max_iter=2000, tol=1e-3)
model.fit(X_train, y_train)

accuracy = model.score(X_test, y_test)
print(f"Accuracy: {accuracy:.4f}")

num_classes = len(class_names)
coef = model.coef_
intercept = model.intercept_

with open('model_weights.h', 'w') as f:
    f.write(f"#define NUM_CLASSES {num_classes}\n")
    f.write("#define NUM_FEATURES 3\n\n")

    f.write("const char* class_names[NUM_CLASSES] = {")
    f.write(", ".join([f"\"{name}\"" for name in class_names]))
    f.write("};\n\n")

    f.write("const float weights[NUM_CLASSES][NUM_FEATURES] = {\n")
    for i in range(num_classes):
        f.write("  {")
        f.write(", ".join([f"{val:.6f}" for val in coef[i]]))
        f.write("},  // " + class_names[i] + "\n")
    f.write("};\n\n")

    f.write("const float bias[NUM_CLASSES] = {")
    f.write(", ".join([f"{val:.6f}" for val in intercept]))
    f.write("};\n")
