import random
import hashlib
import csv
import sys
from datetime import datetime, timedelta

# Configurable parameters
SETTLEMENT_KEY_WIDTH = 60
PARTY_WIDTH = 40
SUBMITTER_WIDTH = 20
YEARS_BACK_MIN = 1
YEARS_BACK_MAX = 9 # n.b. script does not handle negative well before 2000
AMOUNT_MIN = 1000
AMOUNT_MAX = 5000
NUM_RECORDS = int(sys.argv[1]) if len(sys.argv) >= 2 else 5
BASE_NAMES = ["Alice", "Bob", "Carol", "Dave"]
LAST_NAMES = ["Smith", "Johnson", "Lee", "Brown"]
GEN_NAMES = []
GEN_REPEATED = True

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
    name = f"{first_name}{index} {last_name}{index}"
    GEN_NAMES.append(name)
    return name

def generate_amount():
    return random.randint(AMOUNT_MIN, AMOUNT_MAX)

def generate_secret():
    return ''.join(random.choices('0123456789abcdef', k=32))

def hash_value(value):
    hasher = hashlib.sha256()
    hasher.update(value.encode('utf-8'))
    return hasher.hexdigest()

def to_bit_string(value, bits=None, as_int=False):
    if as_int:
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

def fixed_length(s, desired_length):
    # Trim the string if it's longer than the desired length
    if len(s) > desired_length:
        return s[:desired_length]
    # Pad the string with spaces if it's shorter than the desired length
    elif len(s) < desired_length:
        return s.ljust(desired_length)
    # Return the string as is if it's already the correct length
    else:
        return s
        

# Generate up to 3 repeating parties with varying repeating counts
def gen_or_repeat_party(index):
    if GEN_REPEATED and index >= 2 and index <= 3: # repeat 2 additional times
        return GEN_NAMES[0]
    elif GEN_REPEATED and index >= 6 and index <= 8: # repeat 3 additional times
        return GEN_NAMES[5]
    elif GEN_REPEATED and index >= 12 and index <= 15: # repeat 4 additional times
        return GEN_NAMES[9]
    else:
        return generate_name(index)
    
def HACK_chop_value(s, target_len):    
    # HACK: since hashes are currently in hex, 256 bit => 512 bits long,
    # which is inefficient.
    # Therefore, chop them in half so that bits to output is still 256 bits.
    # In the future, when the code is improved to generate raw bytes,
    # can have them output in full-length raw bytes instead of hex,
    # with no change to the MPC program.
    return fixed_length(s, target_len)
    
def main():
    original_submissions = []
    submissions = []
    
    for i in range(NUM_RECORDS):
        date_generated = generate_date()
        signature_date = date_generated.strftime('%Y%m%d')
        year_since_2000 = date_generated.year - 2000        
        day_since_2000 = (date_generated - datetime(2000, 1, 1)).days
        parties = [generate_name(i), gen_or_repeat_party(i)]
        parties.sort()
        party1 = fixed_length(parties[0], PARTY_WIDTH)
        party2 = fixed_length(parties[1], PARTY_WIDTH)
        amount = generate_amount()

        settlement_key = fixed_length("/".join([signature_date, party1, party2]),
                                      SETTLEMENT_KEY_WIDTH)              
        
        for submit_party in [party1, party2]:
          submitter = fixed_length(submit_party, SUBMITTER_WIDTH)
          secret = generate_secret()
          secret_hash = hash_value(secret)
          
          if True: # Hack chop the value
            secret_hash = HACK_chop_value(secret_hash, 32)
            secret = HACK_chop_value(secret, 16)
          
          # This commitment forces the submitters to commit to the settlement key
          # and their names, as authenticated by their secrets.
          # (technically should use original secret before hack chop, but it's same)
          commitment_token = hash_value(settlement_key + submitter + secret)
          commitment_token = HACK_chop_value(commitment_token, 32)

          original_submissions.append([
              day_since_2000,
              party1,
              party2,
              amount,
              year_since_2000,
              signature_date,
              submitter,
              commitment_token,

              # For now, these values are not needed in the MPC computation.
              # Submitters should keep them private to themselves.
              secret_hash,
              secret
          ])
        
          submissions.append([
              to_bit_string(day_since_2000, 16, True),

              to_bit_string(party1, PARTY_WIDTH*8),
              to_bit_string(party2, PARTY_WIDTH*8),

              to_bit_string(amount, 32, True),
              to_bit_string(year_since_2000, 8, True),
              to_bit_string(signature_date, 64),
              to_bit_string(submitter, SUBMITTER_WIDTH*8),
              to_bit_string(commitment_token, 256),
              to_bit_string(secret_hash, 256),
              to_bit_string(secret, 256)
          ])

    # Write submissions to a plaintext file without headers for comparison
    with open('submissions.txt', 'w', newline='') as f:
        writer = csv.writer(f, lineterminator='\n')
        for submission in original_submissions:
            writer.writerow(submission)

    # Convert to bit strings and write to files
    with open('submission_bits.txt', 'w', newline='') as f, \
         open('submission_bits_a.txt', 'w', newline='') as fa, \
         open('submission_bits_b.txt', 'w', newline='') as fb:
        writer = csv.writer(f, lineterminator='\n')
        writer_a = csv.writer(fa, lineterminator='\n')
        writer_b = csv.writer(fb, lineterminator='\n')
        for submission in submissions:
            writer.writerow(submission)
            parts_a, parts_b = zip(*(xor_split(bit) for bit in submission))
            writer_a.writerow(parts_a)
            writer_b.writerow(parts_b)

    print("Generated " + str(NUM_RECORDS) + " settlements");

    if False:
      # Reconstitute and check
      with open('submission_bits_a.txt', 'r') as fa, \
          open('submission_bits_b.txt', 'r') as fb, \
          open('submission_bits_check.txt', 'w', newline='') as f_check:
          reader_a = csv.reader(fa)
          reader_b = csv.reader(fb)
          writer_check = csv.writer(f_check, lineterminator='\n')
          for row_a, row_b in zip(reader_a, reader_b):
              reconstructed = ["".join(str(int(a) ^ int(b)) for a, b in zip(part_a, part_b)) for part_a, part_b in zip(row_a, row_b)]
              writer_check.writerow(reconstructed)

      compare_files('submission_bits_check.txt', 'submission_bits.txt')

if __name__ == "__main__":
    main()
    