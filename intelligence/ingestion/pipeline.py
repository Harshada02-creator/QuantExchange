import os
import sys
import pandas as pd
import hashlib

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..')))
from intelligence.cleaning.validation import validate_ohlcv
from intelligence.storage.repository import save_dataset_metadata, save_historical_ohlcv

def ingest_historical_ohlcv(file_path: str, source: str, description: str):
    """
    Reads a CSV, validates it, creates metadata, and inserts into DB.
    """
    df = pd.read_csv(file_path)
    
    # Validate and clean
    clean_df = validate_ohlcv(df)
    
    # Generate simple version hash
    hash_input = f"{source}_{description}_{clean_df.shape[0]}".encode('utf-8')
    version_hash = hashlib.sha256(hash_input).hexdigest()[:16]
    
    # Save metadata
    save_dataset_metadata(version_hash, source, description)
    
    # Save data
    save_historical_ohlcv(clean_df, version_hash)
    
    return version_hash, len(clean_df)

if __name__ == "__main__":
    fixture_path = os.path.join(os.path.dirname(__file__), '..', 'fixtures', 'sample_ohlcv.csv')
    v_hash, count = ingest_historical_ohlcv(fixture_path, "local_fixture", "Initial baseline mock dataset")
    print(f"Ingested {count} records successfully. Version Hash: {v_hash}")
