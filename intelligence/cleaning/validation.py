import pandas as pd

def validate_ohlcv(df: pd.DataFrame) -> pd.DataFrame:
    """
    Validates and cleans raw OHLCV DataFrame.
    """
    # 1. Missing required fields
    required_cols = ['timestamp', 'symbol', 'open', 'high', 'low', 'close', 'volume']
    for col in required_cols:
        if col not in df.columns:
            raise ValueError(f"Missing required column: {col}")

    df = df.copy()

    # 2. Invalid prices/quantities
    price_cols = ['open', 'high', 'low', 'close']
    for col in price_cols:
        df[col] = pd.to_numeric(df[col], errors='coerce')
        if (df[col] <= 0).any():
            raise ValueError(f"Invalid non-positive price found in column: {col}")
    
    df['volume'] = pd.to_numeric(df['volume'], errors='coerce')
    if (df['volume'] < 0).any():
        raise ValueError("Invalid negative volume found")

    # 3. Invalid timestamps
    df['timestamp'] = pd.to_datetime(df['timestamp'], errors='coerce')
    if df['timestamp'].isnull().any():
        raise ValueError("Invalid unparseable timestamps found")

    # 4. Duplicate records
    duplicates = df.duplicated(subset=['timestamp', 'symbol'])
    if duplicates.any():
        raise ValueError("Duplicate timestamp/symbol records found")

    # 5. Missing values
    if df[required_cols].isnull().any().any():
        df.dropna(subset=required_cols, inplace=True)

    # Sort logically
    df.sort_values(by=['symbol', 'timestamp'], inplace=True)
    
    return df
