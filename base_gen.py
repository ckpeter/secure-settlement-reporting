import random
import hashlib
import csv
from datetime import datetime, timedelta

# Configurable parameters
SETTLEMENT_KEY_WIDTH = 60
SUBMITTER_WIDTH = 20
YEARS_BACK_MIN = 1
YEARS_BACK_MAX = 25
AMOUNT_MIN = 8000
AMOUNT_MAX = 5000000
NUM_RECORDS = 10
BASE_NAMES = ["Alice", "Bob", "Carol", "Dave"]
LAST_NAMES = ["Smith", "Johnson", "Lee", "Brown"]

def generate_date():
    today = datetime.now()
    start = today.replace(year=today.year - YEARS_BACK_MAX)
    end = today.replace(year=today.year - YEARS_BACK_MIN)
    delta = end - start
    random_day = random.randrange(delta.days)
    return (start + timedelta(days=random_day))

def generate_name(index):
    first_name = random.choice(BASE_NAMES) + str(random.randint(1, 999))
    last_name = random.choice(LAST_NAMES) + str(random.randint(1, 999))
    return f"{first_name}{index} {last_name}{index}"

def generate_amount():
    return random.randint(AMOUNT_MIN, AMOUNT_MAX)

def generate_secret():
    return ''.join(random.choices('0123456789abcdef', k=32))

def hash_value(value):
    hasher = hashlib.sha256()
    hasher.update(value.encode('utf-8'))
    return hasher.hexdigest()

def to_bit_string(value, bits=None):
    if isinstance(value, int):
        return format(value, f'0{bits}b')
    else:
        return ''.join(format(ord(c), '08b') for c in value)

def xor_split(bit_string):
    part_a = ''.join(random.choice('01') for _ in bit_string)
    part_b = ''.join(str(int(a) ^ int(b)) for a, b in zip(bit_string, part_a))
    return part_a, part_b

def compare_files(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        if f1.readlines() == f2.readlines():
            print(f"{file1} and {file2} are identical")
        else:
            print(f"{file1} and {file2} are not identical")

def main():
    submissions = []
    for i in range(NUM_RECORDS):
        date_generated = generate_date()
        signature_date = date_generated.strftime('%Y%m%d')
        year_since_1900 = date_generated.year - 1900
        party1 = generate_name(i)
        party2 = generate_name(i + NUM_RECORDS)
        amount = generate_amount()
        secret = generate_secret()
        settlement_key = hash_value(signature_date + party1 + party2)
        commitment_token = hash_value(settlement_key + party1 + secret)
        secret_hash = hash_value(secret)
        
        submissions.append([
            to_bit_string(settlement_key, 512),
            to_bit_string(amount, 32),
            to_bit_string(year_since_1900, 8),
            to_bit_string(signature_date, 64),
            to_bit_string(party1, 160),
            to_bit_string(commitment_token, 256),
            to_bit_string(secret_hash, 256),
            to_bit_string(secret, 256)
        ])

    # Write submissions to a plaintext file without headers for comparison
    with open('submissions.txt', 'w', newline='') as f:
        writer = csv.writer(f)
        for submission in submissions:
            writer.writerow(submission)

    # Convert to bit strings and write to files
    with open('submission_bits.txt', 'w', newline='') as f, \
         open('submission_bits_a.txt', 'w', newline='') as fa, \
         open('submission_bits_b.txt', 'w', newline='') as fb:
        writer = csv.writer(f)
        writer_a = csv.writer(fa)
        writer_b = csv.writer(fb)
        for submission in submissions:
            writer.writerow(submission)
            parts_a, parts_b = zip(*(xor_split(bit) for bit in submission))
            writer_a.writerow(parts_a)
            writer_b.writerow(parts_b)

    # Reconstitute and check
    with open('submission_bits_a.txt', 'r') as fa, \
         open('submission_bits_b.txt', 'r') as fb, \
         open('submission_bits_check.txt', 'w', newline='') as f_check:
        reader_a = csv.reader(fa)
        reader_b = csv.reader(fb)
        writer_check = csv.writer(f_check)
        for row_a, row_b in zip(reader_a, reader_b):
            reconstructed = ["".join(str(int(a) ^ int(b)) for a, b in zip(part_a, part_b)) for part_a, part_b in zip(row_a, row_b)]
            writer_check.writerow(reconstructed)

    compare_files('submission_bits_check.txt', 'submission_bits.txt')

if __name__ == "__main__":
    main()