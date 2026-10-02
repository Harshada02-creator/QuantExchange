import os
import sys
import pandas as pd
import hashlib

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..')))
from intelligence.cleaning.validation import validate_ohlcv
from intelligence.storage.repository import save_dataset_metadata, save_historical_ohlcv

def prepare_nse_bhavcopy(file_path):
    print(f"Reading raw file: {file_path}")
    df = pd.read_csv(file_path)
    raw_rows = len(df)
    print(f"Raw rows: {raw_rows}")
    
    # Filter for Equities only
    if 'SctySrs' in df.columns:
        df = df[df['SctySrs'] == 'EQ'].copy()
        
    # Rename columns to match internal schema
    rename_map = {
        'TradDt': 'timestamp',
        'TckrSymb': 'symbol',
        'OpnPric': 'open',
        'HghPric': 'high',
        'LwPric': 'low',
        'ClsPric': 'close',
        'TtlTradgVol': 'volume'
    }
    df.rename(columns=rename_map, inplace=True)
    
    required_cols = ['timestamp', 'symbol', 'open', 'high', 'low', 'close', 'volume']
    
    # Check if required cols are present
    missing = [c for c in required_cols if c not in df.columns]
    if missing:
        raise ValueError(f"Missing columns after rename: {missing}")
        
    df = df[required_cols]
    
    return df, raw_rows

def ingest_nse(file_path, source, description):
    df, raw_rows = prepare_nse_bhavcopy(file_path)
    
    print("Validating normalized data...")
    clean_df = validate_ohlcv(df)
    normalized_rows = len(clean_df)
    print(f"Normalized valid rows: {normalized_rows}")
    
    print("Computing actual file SHA-256 for provenance...")
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    full_sha256 = sha256_hash.hexdigest()
    
    # Use first 16 chars for metadata tracking
    version_hash = full_sha256[:16]
    
    print(f"Full File SHA-256: {full_sha256}")
    print(f"Short Version Hash: {version_hash}")
    
    # Update description to document the hash origin
    actual_description = f"{description} | File SHA-256: {full_sha256}"
    
    print("Saving to PostgreSQL...")
    save_dataset_metadata(version_hash, source, actual_description)
    save_historical_ohlcv(clean_df, version_hash)
    
    return version_hash, raw_rows, normalized_rows, full_sha256

if __name__ == "__main__":
    raw_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'data', 'raw', 'BhavCopy_NSE_CM_0_0_0_20260930_F_0000.csv'))
    v_hash, raw_count, norm_count, full_sha256 = ingest_nse(
        raw_path,
        source="NSE_CM_UDiFF_20260930",
        description="Official NSE Daily Bhavcopy - UDiFF Format"
    )
    print(f"Successfully ingested {norm_count} rows. Hash: {v_hash}")
