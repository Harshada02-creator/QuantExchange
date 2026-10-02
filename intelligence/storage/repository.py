import psycopg2
import psycopg2.extras
import pandas as pd
import os
import sys

# Ensure intelligence is in path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..')))
from intelligence.config import DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASSWORD

def get_connection():
    return psycopg2.connect(
        host=DB_HOST,
        port=DB_PORT,
        dbname=DB_NAME,
        user=DB_USER,
        password=DB_PASSWORD
    )

def save_dataset_metadata(version_hash: str, source: str, description: str):
    conn = get_connection()
    try:
        with conn.cursor() as cur:
            cur.execute("""
                INSERT INTO dataset_metadata (version_hash, source, description)
                VALUES (%s, %s, %s)
                ON CONFLICT (version_hash) DO NOTHING
            """, (version_hash, source, description))
        conn.commit()
    finally:
        conn.close()

def save_historical_ohlcv(df: pd.DataFrame, version_hash: str):
    conn = get_connection()
    try:
        with conn.cursor() as cur:
            data = [
                (
                    row.timestamp, row.symbol, row.open, row.high,
                    row.low, row.close, row.volume, version_hash
                )
                for row in df.itertuples(index=False)
            ]
            query = """
                INSERT INTO historical_ohlcv (timestamp, symbol, open, high, low, close, volume, provenance_hash)
                VALUES %s
                ON CONFLICT (symbol, timestamp) DO UPDATE SET
                    open = EXCLUDED.open,
                    high = EXCLUDED.high,
                    low = EXCLUDED.low,
                    close = EXCLUDED.close,
                    volume = EXCLUDED.volume,
                    provenance_hash = EXCLUDED.provenance_hash
            """
            psycopg2.extras.execute_values(cur, query, data)
        conn.commit()
    finally:
        conn.close()
