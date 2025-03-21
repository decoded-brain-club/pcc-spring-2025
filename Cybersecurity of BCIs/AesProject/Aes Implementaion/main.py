# main.py

import os
import argparse
from aes_module import encrypt, decrypt  # Adjust the import if needed

def process_file(file_path, key, action):
    # Read file data in binary mode
    with open(file_path, 'rb') as f:
        data = f.read()

    if action == 'encrypt':
        processed_data = encrypt(key, data)
    elif action == 'decrypt':
        processed_data = decrypt(key, data)
    else:
        raise ValueError("Invalid action. Use 'encrypt' or 'decrypt'.")

    # Define output file name
    base, ext = os.path.splitext(file_path)
    if action == 'encrypt':
        out_file = base + '.enc'
    else:  # decrypt
        out_file = base + '.dec' + ext

    # Write the processed data back to file
    with open(out_file, 'wb') as f:
        f.write(processed_data)

    print(f"{action.capitalize()}ed '{file_path}' to '{out_file}'.")

def main():
    parser = argparse.ArgumentParser(
        description="Encrypt or decrypt files in a folder using AES."
    )
    parser.add_argument("action", choices=["encrypt", "decrypt"], help="Action to perform")
    parser.add_argument("key", help="Encryption/Decryption key (string)")
    parser.add_argument("folder", help="Folder containing files to process")
    args = parser.parse_args()

    # Process all files in the specified folder (non-recursive)
    for filename in os.listdir(args.folder):
        file_path = os.path.join(args.folder, filename)
        if not os.path.isfile(file_path):
            continue
        if args.action == "decrypt" and not filename.endswith(".enc"):
            print(f"Skipping non-encrypted file: {filename}")
            continue
        # Optionally, process only files (skip folders)
        if os.path.isfile(file_path):
            process_file(file_path, args.key, args.action)

if __name__ == "__main__":
    main()
